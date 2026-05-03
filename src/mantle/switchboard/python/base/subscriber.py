'''
subscriber.py - Mix-in base class for capturing and intercepting switch updates via method decorators
'''

__author__ = "Tor Slettnes"
__docformat__ = "javadoc en"


### Standard Python modules
from typing import Callable, Set, Sequence, Mapping
from dataclasses import dataclass
import re
import fnmatch

### Common Core modules
from cc.core.decorators import override
from cc.core.invocation import method_path
from cc.protobuf.signal import SignalStore, MappingAction

### Switchboard modules
from ..protobuf import (
    Signal, State, StateMask, StateSet, encodeStateSet,
    InterceptorInvocation,
    ExceptionHandling,
)
from .signals   import switchboard_signals
from .switch    import Switch
from .baseboard import SwitchboardBase
from .observer  import SwitchboardObserver


#-------------------------------------------------------------------------------
# Local type declarations

@dataclass
class DecoratedInterceptorSpec:
    unbound_method: Callable[[InterceptorInvocation], None]
    kwargs: Mapping[str, object]


#-------------------------------------------------------------------------------
# WithSwitchboardInterceptors

class WithSwitchboardInterceptors:
    '''
    Mix-in base class for capturing and intercepting Switchboard updates via
    decorated methods.

    In an AsyncIO event loop is availble, the decorated functions may optionally
    be an AsyncIO coroutines.


    **Example Usage:**

    ```python
    from cc.platfrom.switchboard.protobuf import State, InterceptorInvocation
    from cc.platform.switchboard.base import SwitchboardSubscriber
    from cc.platform.switchboard.grpc import AsyncClient


    class MyClass (SwitchboardSubscriber):

        def __init__(self):
            self.switchboard = AsyncClient("localhost")

        @SwitchboardSubscriber.interceptor('SomeSwitch', State.ACTIVATING)
        async def activate_someswitch(self, invocation: InterceptorInvocation):
            print("Entering switch %r interceptor %r for transition to %r"%(
                  invocation.switch_name,
                  invocation.interceptor_name,
                  invocation.state))
    ```
    '''

    _interceptor_specs: Sequence[DecoratedInterceptorSpec] = []

    def __init__(self, switchboard: SwitchboardBase):
        '''
        Initializer. This will

        - assign `self.swichboard` to the provided Switchboard instance, which
          will be used to communicate with the Switchboard service in order to
          register the decorated interceptors as matching switches are added.

        - invoke `self.register_switchboard_signals()`, which will connect to
          the local signal store in order to:

          * register @interceptor-decorated methods as interceptors for new
            swithes as they are added, and

          * register methods decorated with `@specification_handler` and
            `@status_handler` (defined in the `SwitchboardObserver` parent
            class) for invocation after a specification/s

        In addition, the provided `switcboard` instance must be initialized (via
        `switchboard.initialize()`) in order to actually initiate communication
        with the server and receive updates.
        '''

        self.switchboard = switchboard


    @classmethod
    def interceptor(cls,
                    switch_selection: SwitchSelectionInput,
                    state_transitions: StateMask|StateSet = State.PENDING,
                    phase: InterceptorPhase = InterceptorPhase.NORMAL,
                    asynchronous: bool = False,
                    immediate: bool = True,
                    rerun: bool = False,
                    on_cancel: ExceptionHandling = ExceptionHandling.ABORT,
                    on_error: ExceptionHandling = ExceptionHandling.FAIL):
        '''
        Generate a decorator function to declare an interceptor for Switchboard
        state transitions. The decorated method may optionally return an AsyncIO
        coroutine (as with `async def`).

        The interceptor may raise a `CancelIntercept` exception to indicate that
        the state transition should be cancelled.  In this case, the value of
        `on_cancel` determines what happens next:


          - ExceptionHandling.IGNORE:
            proceed with the state change regardless

          - ExceptionHandling.ABORT:
            return to the previous settled state without further actions

          - ExceptionHandling.FAIL:
            redirect the switch to the `FAILED` state

          - ExceptionHandling.REVERT:
            re-enter the previous settled state anew, invoking any applicable
            interceptors on the way.


        **Inputs:**

        @param pattern
            A switch name, a shell-style pattern containing wildcard characters
            ('*' and/or '?'), or a compiled regular expression object.

        @param state_transitions
            One or more switch states, represented either as a single bitmask or
            a set of states.

        @param phase
            Control where this interceptor fits in the execution order of all
            interceptors for the specified state transitions: EARLY, NORMAL, or
            LATE.

        @param asynchronous
            Allow state to transition to the next state (normally `ACTIVE`,
            `INACTIVE` or `FAILED`) even if this interceptor continues to run
            in the background.  As a side effect, any exceptions raised by the
            interceptor will not affect the status of the switch.

        @param immediate
            Invoke interceptor immediately on registration for any matching
            switch already in the specified state(s)

        @param rerun
            Invoke this intereceptor when (explicitly) re-entering the specified
            state(s), even if the switch is already in that state.

        @param on_cancel
             How to proceed if the state change is cancelled; see above.

        @param on_error
             How to proceed if the state change is cancelled; see above.


        **Example:**

        ```python
        from cc.platform.switchboard.protobuf import State, InerceptorInvocation

        @SwitchboardObserver.interceptor('SomeSwitch', State.ACTIVATING)
        async def activate_someswitch(self, invocation: InterceptorInvocation):
            print("Entering switch %r interceptor %r for transition to %r"%(
                invocation.switch_name,
                invocation.interceptor_name,
                invocation.state))
        ```
        '''

        kwargs = locals()
        del kwargs['cls']

        def decorator(method: Callable[[InterceptorInvocation], None]):
            '''
            Method decorator for Switchboard interceptor
            '''

            cls._interceptor_specs.append(
                DecoratedInterceptorSpec(
                    unbound_method = method,
                    kwargs = kwargs,
                )
            )
            return method

        return decorator


    def register_decorated_interceptors(self, instance: object):
        '''
        Register interceptor decorated interceptor methods with server.
        This must be invoked for those interceptors to become effective.

        @param instance
            Instance of the class that contains decorated interceptors
        '''

        for spec in self._interceptor_specs:
            method_name = spec.unbound_method.__name__
            if getattr(type(instance), method_name, None) == spec.unbound_method:
                bound_method = getattr(instance, method_name)
                self.add_interceptor(
                    interceptor_name = method_path(bound_method)
                    callback = bound_method,
                    future = True,
                    **spec.kwargs)


