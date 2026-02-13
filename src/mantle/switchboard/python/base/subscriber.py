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
from cc.protobuf.dissecter import decode_message
from cc.protobuf.signal import SignalStore, MappingAction

### Switchboard modules
from ..protobuf import (
    Signal, State, StateMask, StateSet, encodeStateSet,
    InterceptorSpec, InterceptorPhase,
    InterceptorRegistration, InterceptorInvocation,
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
    switch_pattern: re.Pattern
    registration: InterceptorRegistration
    unbound_method: Callable[[InterceptorInvocation], None]


#-------------------------------------------------------------------------------
# SwitchboardSubscriber

class SwitchboardSubscriber (SwitchboardObserver):
    '''
    Mix-in base class for capturing and intercepting Switchboard updates via
    decorated methods.

    Any subclass that overrides `__init__()` must invoke the parent
    `Subscriber.__init__(self, switchboard)`; see that method for details.

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
        self.connect_switchboard_signals()


    @override
    def connect_switchboard_signals(self):
        SwitchboardObserver.connect_switchboard_signals(
            self,
            self.switchboard.signal_store)

        self.signal_store.connect_signal(
            self.SPEC_SIGNAL,
            self.__on_switch_spec)

    @override
    def disconnect_switchboard_signals(self):
        self.switchboard.signal_store.disconnect_signal(
            self.SPEC_SIGNAL,
            self.__on_switch_spec)


    @classmethod
    def interceptor(cls,
                    pattern: str|re.Pattern,
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

        registration = InterceptorRegistration(
            spec = InterceptorSpec(
                state_transitions = encodeStateSet(state_transitions),
                phase = phase,
                asynchronous = asynchronous,
                rerun = rerun,
                on_cancel = on_cancel,
                on_error = on_error,
            ),
            immmediate = immmediate,
        )


        def decorator(method: Callable[[InterceptorInvocation], None]):
            '''
            Method decorator for Switchboard interceptor
            '''

            cls._interceptor_specs.append(
                DecoratedInterceptorSpec(
                    switch_pattern = cls._regex_pattern(pattern),
                    registration = registration,
                    unbound_method = method
                )
            )
            return method

        return decorator


    def __on_switch_spec(self, msg: Signal):
        if msg.mapping_action == MappingAction.ADDITION:
            if switch_name := msg.mapping_key:
                self.__register_decorated_interceptors(switch_name)


    def __register_decorated_interceptors(self, switch_name: str):
        for spec in self._interceptor_specs:
            if self.__is_matching_interceptor(switch_name, spec):
                methodname = spec.unbound_method.__name__
                if bound_method := getattr(self, methodname, None):
                    self.switchboard.register_interceptor(
                        switch_name = switch_name,
                        interceptor_name = method_path(bound_method),
                        registration = spec.registration,
                        method = bound_method,
                    )


    def __is_matching_interceptor(self,
                                  switch_name: str,
                                  interceptor_spec: DecoratedInterceptorSpec,
                                  ) -> bool:
        '''
        Determine if a given interceptor decorator is applicable for a switch.
        '''

        unbound_method = interceptor_spec.unbound_method
        method_name = unbound_method.__name__
        return all(
            getattr(type(self), method_name, None) == unbound_method,
            interceptor_spec.switch_pattern.match(switch_name),
        )



