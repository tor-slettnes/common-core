'''
Base class for capturing switch updates via method decorators
'''

### Standard Python modules
from typing import Callable

### Common Core modules
from cc.core.invocation import safe_invoke_maybe_async

### Switchboard modules
from ..protobuf import (
    Signal, State, StateMask, StateSet, encodeStateSet,
)

from .switch    import Switch
from .baseboard import SwitchboardBase

UpdateHandler = Callable[[Switch], None]

class SwitchboardObserver:
    '''
    Mix-in base class for capturing Switchboard updates via decorated methods.

    **Example Usage:**

    ```python
    from cc.platform.switchboard.base import SwitchboardObserver, State
    from cc.platform.switchboard.grpc import Client as SwitchboardClient

    class MyClass (SwitchboardObserver):

        def __init__ (self, switchboard_host: str|None = None):

            ### Create Switchboard instancea
            self.switchboard = SwitchboardClient(switchboard_host)

            ### Connect to and start receiving signals from server
            self.connect_switchboard(self.switchboard)

        @SwitchboardObserver.specification_handler("MySwitch")
        def on_my_switch_active(self, switch: Switch):
            """Handle MySwitch specification change"""

        @SwitchboardObserver.status_handler("MySwitch", State.ACTIVE)
        def on_my_switch_active(self, switch: Switch):
            """Handle MySwitch activation"""
    ```


    '''

    SPEC_SIGNAL = 'specification'
    STATUS_SIGNAL = 'status'


    def connect_switchboard(self, switchboard: SwitchboardBase):
        '''
        '''
        self.connected_switchboard = switchboard

        self.connected_switchboard.signal_store.connect_signal(
            self.SPEC_SIGNAL, self._on_spec_signal)

        self.connected_switchboard.signal_store.connect_signal(
            self.STATUS_SIGNAL, self._on_status_signal)

        self.connected_switchboard.start_watching()


    def disconnect_switchboard(self):
        '''
        '''
        self.connected_switchboard.signal_store.disconnect_signal(
            self.STATUS_SIGNAL, self._on_status_signal)

        self.connected_switchboard.signal_store.disconnect_signal(
            self.SPEC_SIGNAL, self._on_spec_signal)


    def _on_spec_signal(self, msg: Signal):
        if switch := self.connected_switchboard.get_switch(msg.mapping_key):
            self._invoke_specification_handler(switch, msg)

    def _on_status_signal(self, msg: Signal):
        if switch := self.connected_switchboard.get_switch(msg.mapping_key):
            self._invoke_status_handler(switch, msg)



    @classmethod
    def specification_handler(switch_name: str):
        '''
        Return a decorator generator function to handle Switchboard
        specification updates.

        The decorated function may optionally be an AsyncIO coroutine.

        Example:

        ```python
        @SwitchboardObserver.switch_specification_handler('Devices:Online')
        async def on_devices_online_spec(self, switch: Switch):
            # Handle devices online switch specifciation updates
        ```
        '''

        def decorator(function: Callable[[Switch], None]):
            '''
            Function decorator for Switchboard specification updates
            '''
            handler_map = cls._get_handler_map(cls.SPEC_SIGNAL)
            handlers = handler_map.setdefault(switch_name, [])
            handlers.append(callback)
            return function

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
        async def on_devices_online_status(self, switch: Switch):
            # Handle devices online event
        ```
        '''

        def decorator(function: Callable[[Switch], None]):
            '''
            Function decorator for Switchboard status updates
            '''

            handler_map = cls._get_handler_map(cls.STATUS_SIGNAL)
            handlers = handler_map.setdefault(switch_name, {})
            for state in encodeStateSet(states):
                handlers.setdefault(state, []).append(function)

            return function

        return decorator


    def _invoke_specification_handler(self, switch: Switch, msg: Signal):
        if handlers := self._get_handler_map(self.SPEC_SIGNAL).get(msg.mapping_key):
            for handler in handlers:
                safe_invoke_maybe_async(handler, args=(self, switch))


    def _invoke_status_handler(self, switch: Switch, msg: Signal):
        if handlers := self._get_handler_map(self.STATUS_SIGNAL).get(msg.mapping_key):
            if state_handlers := handlers.get(msg.status.current_state):
                for handler in state_handlers:
                    safe_invoke_maybe_async(handler, args=(self, switch))


    @classmethod
    def _get_handler_map(cls, signal_name) -> dict:
        '''
        Update handlers are populated via function decorators at module load
        time.  This means if the process gets forked, the populated map is also
        inherited.

        This method ensure we create a clean map of update handlers for each
        process.
        '''
        if cls._handler_map == None:
            cls._handler_map = {
                cls.SPEC_SIGNAL: {},
                cls.STATUS_SIGNAL: {},
            }

        return cls._handler_map.get(signal_name)


    _handler_map = None
