#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file signal_slot.py
## @brief Signal/Slot pattern implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .invocation import safe_invoke, check_type

### Standard Pyton modules
from typing import Optional, Callable, Mapping, Any
import uuid
import threading

#===============================================================================
# Annotation types

SignalSlot  = Callable[[Any], None]


#===============================================================================
# SignalStore class

class DataSignal (object):

    def __init__(self, signal_name: str, signal_type : type, use_cache = False):
        self.signal_name = signal_name
        self.signal_type = signal_type
        self.use_cache = use_cache
        self.slots = {}
        self._cached = None

    def connection_count(self) -> int:
        '''Return number of connected slots'''
        return len(self.slots)


    def connect(self,
                slot: SignalSlot,
                handle : Optional[str] = None) -> str:
        '''Connect a callback handler (slot) to receive emitted signals.

        @param[in] slot
            A callable handler (e.g. a function) that accepts the payload
            as its first and only required argument.

        @param[in] handle
            A unique ID that can later be used to disconnect the handler from
            this signal.

        Returns the handle that was either provided or generated
        '''

        assert callable(slot), \
            "Slot must be a callable object, like a function"

        if not handle:
            handle = uuid.uuid1()

        self.slots[handle] = slot
        if self.use_cache:
            self._emit_cached_to(handle, slot)

        return handle


    def disconnect(self, handle: str) -> bool:
        '''Disconnect a simple handler from this signal.

        @param[in] handle
           Signal handler

        Returns True if the handler was found and removed; False otherwise.
        '''

        try:
            del self.slots[handle]
            return True
        except KeyError:
            return False

    def emit(self, data: Any):
        for handle, callback in self.slots.items():
            self._emit_to(handle, callback, data)

        if self.use_cache:
            self._cached = data

    def _emit_cached_to(self, handle: str, slot: SignalSlot):
        if self._cached is not None:
            self._emit_to(handle, slot, self._cached)

    def _emit_to(self, handle, slot, signal):
        safe_invoke(slot, (signal,), {},
                    "Signal %r handle %r: %s()"%(self.signal_name, handle, slot.__name__))

