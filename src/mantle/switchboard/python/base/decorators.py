'''
Function decorators for switch update handlers
'''

from typing import Callable
from ..protobuf import State, StateMask, StateSet
from .baseboard import Switch, SwitchboardBase


def switch_specification_handler(switch_name: str):
    '''
    Return a decorator generator function to handle Switchboard
    specification updates.

    The decorated function may optionally be an AsyncIO coroutine.

    Example:

    ```python
    @switch_specification_handler('Devices:Online')
    async def on_devices_online_spec(self, switch: Switch):
        # Handle devices online switch specifciation updates
    ```
    '''

    def status_handler_decorator(function: Callable[[Switch], None]):
        '''
        Function decorator for Switchboard status updates
        '''
        SwitchboardBase.add_status_handler(function, switch_name, states)
        return function

    return status_handler_decorator


def switch_status_handler(switch_name: str,
                          states: StateMask|StateSet = State.SETTLED):
    '''
    Return a decorator generator function to handle Switchboard status
    updates.  The decorated function may optionally be an AsyncIO coroutine.

    Example:

    ```python
    @switch_status_handler('Devices:Online', State.ACTIVE)
    async def on_devices_online_status(self, switch: Switch):
        # Handle devices online event
    ```
    '''

    def status_handler_decorator(function: Callable[[Switch], None]):
        '''
        Function decorator for Switchboard status updates
        '''
        print("status_handler_decorator.add_status_handler(%r, %s, %s)" % (
            function.__name__,
            switch_name,
            states))

        SwitchboardBase.add_status_handler(function, switch_name, states)
        return function

    return status_handler_decorator

