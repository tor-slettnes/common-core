'''
Base class for capturing switch updates via method decorators
'''

__author__ = "Tor Slettnes"
__docformat__ = "javadoc en"

### Standard Python modules
from typing import Callable, Sequence, Set
from dataclasses import dataclass
import re
import fnmatch

### Common Core modules
from cc.core.docbase import DocBase
from cc.core.logbase import LogBase
from cc.core.invocation import method_path, invoke_background
from cc.protobuf.signal import MappingAction

### Switchboard modules
from ..protobuf import (
    Signal, State, StateMask, StateSet, encodeStateSet,
    SwitchboardDissecter,
)

from .signals   import switchboard_signals, CachingSignalStore
from .switch    import Switch

@dataclass
class HandlerSpec:
    pattern: re.Pattern
    actions: Set[MappingAction]
    unbound_method: Callable[[Signal], None]
    states: Set[State]|None = None

MAP_UPDATE = {MappingAction.ADDITION, MappingAction.UPDATE}


class SwitchboardObserver (DocBase, LogBase, SwitchboardDissecter):
    '''
    Mix-in base class for capturing Switchboard updates via decorated methods.

    In order to actually receive updates, the object that contains the decorated
    methods must invoke `self.connect_decorated_handlers()`.

    **Example Usage:**

    ```python
    from cc.platform.switchboard.base import SwitchboardObserver, Signal, State


    class MyClass (SwitchboardObserver):

        def __init__(self):
            self.connect_decorated_handlers()

        @SwitchboardObserver.specification_handler("MySwitch")
        def on_my_switch_spec_update(self, signal: Signal):
            """Handle MySwitch specification change"""
            print(f"Updated switch {signal.mapping_key} spec: {signal.specification}")

        @SwitchboardObserver.status_handler("MySwitch", State.ACTIVE|State.INACTIVE)
        def on_my_switch_status_update(self, signal: Signal):
            """Handle MySwitch activation or deactivation"""
            print(f"Updated switch {signal.mapping_key} status: {signal.status}")

    ```

    In an AsyncIO event loop is availble, the decorated functions may optionally
    be AsyncIO coroutines.
    '''

    SPEC_SIGNAL = 'specification'
    STATUS_SIGNAL = 'status'

    _spec_handlers = []
    _status_handlers = []

    signal_store = switchboard_signals

    @classmethod
    def specification_handler(cls,
                              pattern: str|re.Pattern,
                              actions: Set[MappingAction] = MAP_UPDATE):
        '''
        Generate a decorator function to register a handler for Switchboard
        specification updates. The decorated method may optionally return an
        AsyncIO coroutine (as with `async def`).

        In order to actually receive updates, the object that contains the
        decorated methods must invoke `self.connect_decorated_handlers()`.

        **Inputs:**

        @param pattern
            A switch name, a shell-style pattern containing wildcard characters
            ('*' and/or '?'), or a compiled regular expression object.

        @param actions
            Either a single `cc.protobuf.signal.MappingAction` value
            (`ADDITION`, `UPDATE`, and/or `REMOVAL`) or a set of these.

        **Example:**

        ```python
        from cc.platform.switchboard.protobuf import Signal

        @SwitchboardObserver.specification_handler('Device:*:Online')
        async def on_device_online_spec(self, msg: Signal):
            device_name = ' '.join(msg.mapping_key.split(':')[1:-1])
            print("Device %s online switch created: {msg.specification}")
        ```
        '''

        def decorator(method: Callable[[Signal], None]):
            '''
            Function decorator for Switchboard specification updates
            '''
            handler = HandlerSpec(
                pattern = cls._regex_pattern(pattern),
                actions = actions,
                unbound_method = method)

            cls._spec_handlers.append(handler)
            return method

        return decorator


    @classmethod
    def status_handler(cls,
                       pattern: str|re.Pattern,
                       states: StateMask|StateSet = State.SETTLED,
                       actions: Set[MappingAction] = MAP_UPDATE,
                       ):
        '''
        Generate a decorator function to register a handler for Switchboard
        status updates. The decorated method may optionally return an AsyncIO
        coroutine (as with `async def`).

        In order to actually receive updates, the object that contains the
        decorated methods must invoke `self.connect_decorated_handlers()`.

        **Inputs:**

        @param pattern
            A switch name, a shell-style pattern containing wildcard characters
            ('*' and/or '?'), or a compiled regular expression object.

        @param states
            One or more switch states, represented either as a single bitmask or
            a set of states.

        @param actions
            Either a single `cc.protobuf.signal.MappingAction` value
            (`ADDITION`, `UPDATE`, and/or `REMOVAL`) or a set of these.

        **Example:**

        ```python
        from cc.platform.switchboard.protobuf import Signal, State

        @SwitchboardObserver.status_handler('Device:*:Online', State.ACTIVE|State:INACTIVE)
        async def on_devices_online_status(self, msg: Signal):
            device_name = ' '.join(msg.mapping_key.split(':')[1:-1])
            print(f"Device {device_name} online status is now {msg.status.active}")
        ```
        '''

        def decorator(method: Callable[[Signal], None]):
            '''
            Function decorator for Switchboard status updates
            '''

            handler = HandlerSpec(
                pattern = cls._regex_pattern(pattern),
                actions = actions,
                states = encodeStateSet(states),
                unbound_method = method)

            cls._status_handlers.append(handler)
            return method

        return decorator



    @classmethod
    def _regex_pattern(cls, pattern: str|re.Pattern) -> re.Pattern:
        '''
        Translate a provided pattern to a compiled regular expression
        object.
        '''
        if isinstance(pattern, str):
            return re.compile(fnmatch.translate(pattern))
        elif isinstance(pattern, re.Pattern):
            return pattern
        else:
            raise TypeError(
                "'pattern' must be of type 'str' or 're.Pattern', got %r: %s"%(
                    type(pattern).__name__,
                    pattern,
                ))



    def __del__(self):
        self.disconnect_decorated_handlers()


    def connect_switchboard_signals(self):
        '''
        Connect decorated handler methods from this object instance to the
        Switchboard signal store in order to start observing update events.
        '''
        self.connect_decorated_handlers(self)


    def connect_decorated_handlers(self, instance: object):
        '''
        Connect decorated handler methods from the provided object instance
        to the Switchboard signal store in order to start observing update
        events.
        '''
        self._connect_handlers(instance, self.SPEC_SIGNAL, self._spec_handlers)
        self._connect_handlers(instance, self.STATUS_SIGNAL, self._status_handlers)


    def _connect_handlers(self,
                          instance: object,
                          signal_name: str,
                          handlers: list[HandlerSpec]):

        for handler in handlers:
            method_name = handler.unbound_method.__name__
            if getattr(type(instance), method_name, None) == handler.unbound_method:
                if bound_method := getattr(instance, method_name, None):
                    self.signal_store.connect_signal(
                        signal_name,
                        self._invoke_handler,
                        kwargs = dict(
                            method = bound_method,
                            handler = handler,
                        ))

    def _invoke_handler(self,
                        signal: Signal,
                        method: Callable[[Signal], None],
                        handler: HandlerSpec):

        if self._signal_matches(signal, handler):
            invoke_background(
                method,
                args=(signal,),
                description="switch %r %s handler: %r" % (
                    signal.mapping_key,
                    self.signal_store.signal_name(signal),
                    method_path(method),
                ),
                log_call = self.logger.debug,
                log_failure = self.logger.error,
            )


    def _signal_matches(self,
                        signal: Signal,
                        handler: HandlerSpec):
        return all((
            signal.mapping_action in handler.actions,
            (handler.states is None) or (signal.status.current_state in handler.states),
            handler.pattern.match(signal.mapping_key),
        ))
