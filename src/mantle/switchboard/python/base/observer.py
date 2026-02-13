'''
Base class for capturing switch updates via method decorators
'''

__author__ = "Tor Slettnes"
__docformat__ = "javadoc en"

### Standard Python modules
from typing import Callable, Set
from dataclasses import dataclass
import re
import fnmatch

### Common Core modules
from cc.core.invocation import safe_invoke_maybe_async
from cc.protobuf.signal import MappingAction

### Switchboard modules
from ..protobuf import (
    Signal, State, StateMask, StateSet, encodeStateSet,
)

from .signals   import switchboard_signals, CachingSignalStore
from .switch    import Switch

@dataclass
class HandlerSpec:
    pattern: re.Pattern
    actions: Set[MappingAction]
    method: Callable[[Switch], None]
    states: Set[State]|None = None

MAP_UPDATE = {MappingAction.ADDITION, MappingAction.UPDATE}


class SwitchboardObserver:
    '''
    Mix-in base class for capturing Switchboard updates via decorated methods.

    In order to actually receive updates, the object that contains the decorated
    methods must invoke `self.connect_switchboard_signals()`.

    **Example Usage:**

    ```python
    from cc.platform.switchboard.base import SwitchboardObserver, Signal, State


    class MyClass (SwitchboardObserver):

        def __init__(self):
            self.connect_switchboard_signals()

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

    signal_store = None


    @classmethod
    def specification_handler(cls,
                              pattern: str|re.Pattern,
                              actions: Set[MappingAction] = MAP_UPDATE):
        '''
        Generate a decorator function to register a handler for Switchboard
        specification updates. The decorated method may optionally return an
        AsyncIO coroutine (as with `async def`).

        In order to actually receive updates, the object that contains the
        decorated methods must invoke `self.connect_switchboard_signals()`.

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

        @SwitchboardObserver.specification_handler('Devices:*:Online')
        async def on_device_online_spec(self, msg: Signal):
            device_name = ' '.join(msg.mapping_key.split(':')[1:-1])
            print("Device %s online switch created: {msg.specification}")
        ```
        '''

        def decorator(method: Callable[[Switch], None]):
            '''
            Function decorator for Switchboard specification updates
            '''
            handler = HandlerSpec(
                pattern = cls._regex_pattern(pattern),
                actions = actions,
                method = method)

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
        decorated methods must invoke `self.connect_switchboard_signals()`.

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

        def decorator(method: Callable[[Switch], None]):
            '''
            Function decorator for Switchboard status updates
            '''

            handler = HandlerSpec(
                pattern = cls._regex_pattern(pattern),
                actions = actions,
                states = encodeStateSet(states),
                method = method)

            cls._status_handlers.append(handler)
            return method

        return decorator


    def __del__(self):
        self.disconnect_switchboard_signals()

    def connect_switchboard_signals(self,
                                    signal_store: CachingSignalStore = switchboard_signals):
        '''
        Connect to Switchboard signal store to start observing switchboard
        update events.
        '''

        signal_store.connect_signal(
            self.SPEC_SIGNAL,
            self._invoke_specification_handlers)

        signal_store.connect_signal(
            self.STATUS_SIGNAL,
            self._invoke_status_handlers)

        self.signal_store = signal_store


    def disconnect_switchboard_signals(self):
        '''
        Disconnect from Switchboard signal store.
        '''

        if signal_store := self.signal_store:
            signal_store.disconnect_signal(
                self.STATUS_SIGNAL,
                self._invoke_status_handlers)

            signal_store.disconnect_signal(
                self.SPEC_SIGNAL,
                self._invoke_specification_handlers)

            self.signal_store = None


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

    def _handler_matches(self, handler: HandlerSpec, msg: Signal) -> bool:
        '''
        Determine whether a decorated function should receive a switch
        update.
        '''
        return all((
            getattr(type(self), handler.method.__name__, None) == handler.method,
            msg.mapping_action in handler.actions,
            (handler.states is None) or (msg.status.current_state in handler.states),
            handler.pattern.match(msg.mapping_key),
        ))

    def _invoke_specification_handlers(self, msg: Signal):
        for handler in self._spec_handlers:
            if self._handler_matches(handler, msg):
                safe_invoke_maybe_async(handler.method, args=(self, msg))

    def _invoke_status_handlers(self, msg: Signal):
        for handler in self._status_handlers:
            if self._handler_matches(handler, msg):
                safe_invoke_maybe_async(handler.method, args=(self, msg))

