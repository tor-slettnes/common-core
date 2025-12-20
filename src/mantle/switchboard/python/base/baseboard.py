'''
Abstract switchboard implementation.
'''

__all__ = ['SwitchboardBase']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from abc import abstractmethod
from threading import Lock
from logging import Logger

### Core modules
from cc.core.logbase import LogBase
from cc.core.paths import FilePathInput
from cc.core.settingsstore import SettingsStore
from cc.protobuf.dissecter import message_dissecter
from cc.protobuf.signal import SignalStore, MappingAction
from cc.protobuf.variant import PyValueList

### Modules within package
from ..protobuf import Signal
from .switch import Switch
from .signals import switchboard_signals

class SwitchboardBase (LogBase):
    '''
    Switchboard abstract base
    '''

    signal_store = switchboard_signals

    def __init__(self, logger: Logger|None = None):
        LogBase.__init__(self, logger = logger)

        self.switches = {}
        self._switch_lock = Lock()
        self._connect_signals()

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
            with self._switch_lock:
                match msg.mapping_action:
                    case MappingAction.ADDITION | MappingAction.UPDATE:
                        try:
                            switch = self.switches[switch_name]
                        except KeyError:
                            switch = self.switches[switch_name] = self._new_switch(switch_name)

                    case MappingAction.REMOVAL:
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
                   ) -> Switch|None:
        '''
        Get the named switch.

        @returns
            The named `Switch` instance if it exists, otherwise `None`.
        '''

        if switch := self.switches.get(switch_name):
            return switch

        for name, switch in self.switches.items():
            if switch_name in switch.aliases:
                return switch

        return None


    def get_or_add_switch(self,
                          switch_name: str,
                          initial_value: bool|None = None,
                          ) -> Switch:
        '''
        Get the named switch, or create it if missing.

        @returns
            An existing or new `Switch`
        '''

        with self._switch_lock:
            switch = self.get_switch(switch_name)

            if switch is None:
                self.add_switch(switch_name)
                switch = self.switches[switch_name] = self._new_switch(switch_name)
                if initial_value is not None:
                    switch.set_active(initial_value)

            return switch


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

    @abstractmethod
    def import_switches(self,
                        declarations: PyValueList) -> int:
        '''
        Import switches from a list of key/value declarations, like those
        found in settings files.

        @param declarations
            A list of key/value objects, like those read from a settings file.

        @return
            Number of switches that were added
        '''

    def load_switches(self,
                      filename: FilePathInput):
        '''
        Load switches from a settings file.  Valid file formats are those
        supported by `cc.core.settingsstore.SettingsStore`, including JSON,
        YAML, and INI.

        The settings schema is described in `common-swiches.yaml`, normally
        installed under `/usr/share/common-core/settings/switches/`.
        At the root of the settings tree, a `"switches"` key should map to a
        list of declarations containing, at minimum, a `"name"` key.
        '''

        store = SettingsStore(filename)
        if declarations := store.get_value('switches',
                                           expected_type=list,
                                           raise_invalid_type=True):
            self.import_switches(declarations)
