'''
Base class for capturing switch updates via method decorators
'''

### Standard Python modules
from typing import Callable
from weakref import ref
import sys, os

### Common Core modules
from cc.core.invocation import safe_invoke_maybe_async

### Switchboard modules
from ..protobuf import (
    Signal, State, StateMask, StateSet, encodeStateSet,
)

from .signals   import switchboard_signals
from .switch    import Switch

UpdateHandler = Callable[[Switch], None]

class SwitchboardObserver:
    '''
    Mix-in base class for capturing Switchboard updates via decorated methods.

    **Example Usage:**

    ```python
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
        SPEC_SIGNAL: {},
        STATUS_SIGNAL: {},
    }

    @classmethod
    def specification_handler(cls,
                              switch_name: str):
        '''
        Return a decorator generator function to handle Switchboard
        specification updates.

        The decorated method must be a member of the SwitchboardObserver
        subclass that inherits this method, and may optionally be an AsyncIO
        coroutine.

        Example:

        ```python
        @SwitchboardObserver.specification_handler('Devices:Online')
        async def on_devices_online_spec(self, msg: Signal):
            # Handle devices online switch specifciation updates
        ```
        '''

        def decorator(method: Callable[[Switch], None]):
            '''
            Function decorator for Switchboard specification updates
            '''
            handler_map = self._handler_map[cls.SPEC_SIGNAL]
            handlers = handler_map.setdefault(switch_name, [])
            handlers.append(method)
            return method

        return decorator


    @classmethod
    def status_handler(cls,
                       switch_name: str,
                       states: StateMask|StateSet = State.SETTLED):
        '''
        Return a decorator generator function to handle Switchboard status
        updates.  The decorated function may optionally be an AsyncIO coroutine.

        Example:

        ```python

        @SwitchboardObserver.status_handler('Devices:Online', State.ACTIVE)
        async def on_devices_online_status(self, msg: Signal):
            # Handle devices online event
        ```
        '''

        def decorator(method: Callable[[Switch], None]):
            '''
            Function decorator for Switchboard status updates
            '''
            handler_map = cls._handler_map[cls.STATUS_SIGNAL]
            handlers = handler_map.setdefault(switch_name, {})
            for state in encodeStateSet(states):
                handlers.setdefault(state, []).append(method)

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
            self._invoke_specification_handler)

        switchboard_signals.connect_signal(
            self.STATUS_SIGNAL,
            self._invoke_status_handler)


    def disconnect_switchboard_signals(self):
        '''
        Disconnect from Switchboard signal store.
        '''

        switchboard_signals.disconnect_signal(
            self.STATUS_SIGNAL,
            self._invoke_status_handler)

        switchboard_signals.disconnect_signal(
            self.SPEC_SIGNAL,
            self._invoke_specification_handler)


    def _invoke_specification_handler(self, msg: Signal):
        if handlers := self._handler_map[self.SPEC_SIGNAL].get(msg.mapping_key):
            for handler in handlers:
                if getattr(type(self), handler.__name__, None) == handler:
                    safe_invoke_maybe_async(handler, args=(self, msg))


    def _invoke_status_handler(self, msg: Signal):
        if handlers := self._handler_map[self.STATUS_SIGNAL].get(msg.mapping_key):
            if state_handlers := handlers.get(msg.status.current_state):
                for handler in state_handlers:
                    if getattr(type(self), handler.__name__, None) == handler:
                        safe_invoke_maybe_async(handler, args=(self, msg))


