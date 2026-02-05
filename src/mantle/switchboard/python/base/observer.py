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

from .signals   import switchboard_signals
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

    **Example Usage:**

    ```python
    from cc.protobuf.signal import MappingAction as Action
    from cc.platform.switchboard.base import SwitchboardObserver, Signal, State


    class MyClass (SwitchboardObserver):

        def __init__ (self):
            ### Connect to and start receiving switchboard signals
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

    _handler_map = {
        SPEC_SIGNAL: [],
        STATUS_SIGNAL: [],
    }

    @classmethod
    def specification_handler(cls,
                              pattern: str|re.Pattern,
                              actions: Set[MappingAction] = MAP_UPDATE):
        '''
        Generate a decorator function to register a handler for Switchboard
        specification updates.  The decorated function may optionally be an
        AsyncIO coroutine.


        The decorated method must be a member of the SwitchboardObserver
        subclass that inherits this method, and may optionally be an AsyncIO
        coroutine.

        **Inputs:**

        @param pattern
            A switch name, a shell-style pattern containing wildcard characters
            ('*' and/or '?'), or a compiled regular expression object.

        @param actions
            Either a single `cc.protobuf.signal.MappingAction` value
            (`ADDITION`, `UPDATE`, and/or `REMOVAL`) or a set of these.

        **Example:**

        ```python
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

            cls._handler_map[cls.SPEC_SIGNAL].append(handler)
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
        status updates.  The decorated function may optionally be an AsyncIO
        coroutine.

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
        from cc.platform.switchboard.protobuf import Signal

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

            cls._handler_map[cls.STATUS_SIGNAL].append(handler)
            return method

        return decorator


    def __del__(self):
        self.disconnect_switchboard_signals()


    def connect_switchboard_signals(self):
        '''
        Connect to Switchboard signal store to start observing switchboard
        update events.
        '''
        switchboard_signals.connect_signal(
            self.SPEC_SIGNAL,
            self._invoke_specification_handlers)

        switchboard_signals.connect_signal(
            self.STATUS_SIGNAL,
            self._invoke_status_handlers)


    def disconnect_switchboard_signals(self):
        '''
        Disconnect from Switchboard signal store.
        '''

        switchboard_signals.disconnect_signal(
            self.STATUS_SIGNAL,
            self._invoke_status_handlers)

        switchboard_signals.disconnect_signal(
            self.SPEC_SIGNAL,
            self._invoke_specification_handlers)


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
            msg.mapping_key,
            handler.pattern.match(msg.mapping_key),
            msg.mapping_action in handler.actions,
            (handler.states is None) or (msg.status.current_state in handler.states),
            getattr(type(self), handler.method.__name__, None) == handler.method,
        ))

    def _invoke_specification_handlers(self, msg: Signal):
        for handler in self._handler_map[self.SPEC_SIGNAL]:
            if self._handler_matches(handler, msg):
                safe_invoke_maybe_async(handler.method, args=(self, msg))

    def _invoke_status_handlers(self, msg: Signal):
        for handler in self._handler_map[self.STATUS_SIGNAL]:
            if self._handler_matches(handler, msg):
                safe_invoke_maybe_async(handler.method, args=(self, msg))

