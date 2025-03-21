'''
Abstract switchboard implementation.
'''

__all__ = ['SwitchboardBase']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


### Modules within package
from .switch import Switch
from .signals import signal_store
from cc.protobuf.switchboard import Signal
from cc.protobuf.signal import SignalStore, MappingAction
from abc import abstractmethod

class SwitchboardBase:
    '''
    Switchboard abstract base
    '''

    def __init__(self,
                 signal_store: SignalStore = signal_store):

        self.switches = {}
        self.signal_store = signal_store
        self._connect_signals()

    def __del__(self):
        self._disconnect_signals()

    def _connect_signals(self):
        self.signal_store.connect_signal('specification', self._on_signal_spec)
        self.signal_store.connect_signal('status', self._on_signal_status)

    def _disconnect_signals(self):
        self.signal_store.disconnect_signal('specification', self._on_signal_spec)
        self.signal_store.disconnect_signal('status', self._on_signal_status)

    def _on_signal_spec(self, msg: Signal):
        if switch := self._get_mapped_switch(msg):
            switch._update_specification(msg.specification)

    def _on_signal_status(self, msg: Signal):
        if switch := self._get_mapped_switch(msg):
            switch._update_status(msg.status)

    def _get_mapped_switch(self, msg: Signal) -> Switch|None:
        switch = None

        if switch_name := msg.mapping_key:
            if ((msg.mapping_action == MappingAction.MAP_ADDITION) or
                (msg.mapping_action == MappingAction.MAP_UPDATE)):
                try:
                    switch = self.switches[switch_name]
                except KeyError:
                    switch = self.switches[switch_name] = self._new_switch(switch_name)

            elif (msg.mapping_action == MappingAction.MAP_REMOVAL):
                switch = self.switches.pop(switch_name, None)

        return switch


    @abstractmethod
    def _new_switch(self, switch_name: str) -> Switch:
        '''
        Create a new (derived) Switch object without adding it to the board.
        Intended for local updates in response to server signals.  Must be
        overridden in subclasses.
        '''

    def get_switch(self,
                   switch_name: str,
                   ) -> Switch:
        '''
        Get the named switch.

        @returns
            The named `Switch` instance if it exists, otherwise `None`.
        '''
        return self.switches.get(switch_name)

    @abstractmethod
    def add_switch(self,
                   switch_name: str,
                   ) -> bool:
        '''
        Add a new switch.  Initially this state will have no specifications
        (including dependencies) or status; use appropriate methods on the
        returned `Switch` object (such as `set_specification()` and
        `set_target()`) to populate those.

        @param switch_name:
            Name for the new switch.

        @returns
            True if this switch was added, False if it already existed.
        '''

    @abstractmethod
    def remove_switch(self,
                      switch_name: str,
                      propagate: bool = True,
                      ) -> bool:
        '''
        Remove an existing switch.

        @param switch_name:
            Name of switch to remove.

        @param propagate:
            Automatically update the state of successors (downstream dependents)
            once this switch has been removed.

        @returns
            True if the switch was removed, False if it did not exist.
        '''
