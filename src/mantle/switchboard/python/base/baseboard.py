'''
Abstract switchboard implementation.
'''

__all__ = ['SwitchboardBase']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from abc import abstractmethod
from typing import Callable
from threading import Lock
from logging import Logger
import os

### Core modules
from cc.core.docbase import DocBase
from cc.core.logbase import LogBase
from cc.core.paths import FilePathInput
from cc.core.invocation import safe_invoke_maybe_async
from cc.core.settingsstore import SettingsStore
from cc.protobuf.dissecter import message_dissecter
from cc.protobuf.signal import SignalStore, MappingAction
from cc.protobuf.variant import PyValueList

### Modules within package
from ..protobuf import (
    Signal, InterceptorUpdate,
    State, StateMask, StateSet, encodeStateSet,
)
from .switch import Switch
from .signals import switchboard_signals

class SwitchboardBase (DocBase, LogBase):
    '''
    Switchboard abstract base
    '''

    signal_store = switchboard_signals
    SPEC_SIGNAL = 'specification'
    STATUS_SIGNAL = 'status'


    def __init__(self, logger: Logger|None = None):
        LogBase.__init__(self, logger = logger)

        self.switches = {}
        self._switch_lock = Lock()
        self._connect_signals()

    def _connect_signals(self):
        self.signal_store.connect_signal(
            self.SPEC_SIGNAL,
            self._on_signal_spec)

        self.signal_store.connect_signal(
            self.STATUS_SIGNAL,
            self._on_signal_status)

    def _disconnect_signals(self):
        self.signal_store.disconnect_signal(
            self.SPEC_SIGNAL,
            self._on_signal_spec)

        self.signal_store.disconnect_signal(
            self.STATUS_SIGNAL,
            self._on_signal_status)

    def _on_signal_spec(self, msg: Signal):
        if msg.mapping_action == MappingAction.REMOVAL:
            self.switches.pop(msg.mapping_key, None)

        elif switch := self._get_or_map_switch(msg):
            switch.update_specification(msg.specification)

    def _on_signal_status(self, msg: Signal):
        if msg.mapping_action == MappingAction.REMOVAL:
            self.switches.pop(msg.mapping_key, None)

        elif switch := self._get_or_map_switch(msg):
            switch.update_status(msg.status)

    def _get_or_map_switch(self, msg: Signal) -> Switch|None:
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
        Create a local Switch object without adding it to the board.
        Intended for local updates in response to server signals.
        '''

    def get_switch(self,
                   switch_name: str,
                   required: bool = False,
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

        if required:
            raise KeyError(f"No such switch: {switch_name}")

        return None


    @abstractmethod
    def get_or_add_switch(self,
                          switch_name: str,
                          initially_active: bool = False,
                          ) -> Switch:
        '''
        Get the named switch, or create it if missing.

        @returns
            An existing or new `Switch`
        '''


    @abstractmethod
    def add_switch(self,
                   switch_name: str,
                   active: bool = False,
                   ) -> bool:
        '''
        Add a new switch.  Initially this state will have no specifications
        (including dependencies) or status; use appropriate methods on the
        returned `Switch` object (such as `set_specification()` and
        `set_target()`) to populate those.

        @param switch_name
            Name for the new switch.

        @param active
            Initial active value

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

        @param switch_name
            Name of switch to remove.

        @param propagate
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
        declarations = store.get_value(
            'switches',
            expected_type=list,
            raise_invalid_type=True)
        return self.import_switches(declarations)
