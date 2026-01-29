'''
datasignal.py - Simple signal/slot implementation
'''

__author__ = "Tor Slettnes"
__docformat__ = "javadoc en"


### Standard Python modules
from typing import  Callable, Mapping, Any
import uuid

### Modules within package
from ..invocation import safe_invoke, check_type


#===============================================================================
# Annotation types

SignalSlot  = Callable[[Any], None]


#===============================================================================
# DataSignal class

class DataSignal:
    '''
    Simple signal/slot implementation with payload.
    '''

    def __init__(self,
                 signal_name: str):
        self.signal_name = signal_name
        self.slots = {}

    def connection_count(self) -> int:
        '''
        Return number of connected slots
        '''
        return len(self.slots)

    def connect(self,
                callback: SignalSlot,
                handle : str|None = None) -> str:
        '''
        Connect a callback handler (slot) to receive emitted signals.

        @param callback
            A callable handler (e.g. a function) that accepts the payload
            as its first and only required argument.

        @param handle
            A unique ID that can later be used to disconnect the handler from
            this signal.

        Returns the handle that was either provided or generated
        '''

        assert callable(callback), \
            "Callback must be a callable object, like a function"

        if not handle:
            handle = uuid.uuid1()

        self.slots[handle] = callback
        return handle


    def disconnect(self, handle: str) -> bool:
        '''
        Disconnect a simple handler from this signal.

        @param handle
           Signal handler

        Returns True if the handler was found and removed; False otherwise.
        '''

        try:
            del self.slots[handle]
            return True
        except KeyError:
            return False

    def disconnect_all(self) -> bool:
        if self.slots:
            self.slots.clear()
            return True
        else:
            return False

    def emit(self, data: Any):
        '''
        Emit signal data to connected slots.
        '''
        for handle, callback in self.slots.items():
            self._emit_to(handle, callback, data)

    def _emit_to(self, handle, slot, signal):
        return safe_invoke(
            slot,
            (signal,),
            {},
            "Signal %r handle %r: %s()"%(
                self.signal_name,
                handle,
                slot.__name__,
            ))
