'''
aio_signal.py - Simple signal/slot implementation with AsyncIO emit()
'''

### Standard Python modules
from typing import Any
import asyncio

### Modules witin this package
from ..decorators import doc_inherit
from .datasignal import DataSignal, SignalSlot

class AsyncDataSignal (DataSignal):
    '''
    Simple signal/slot implementation with payload and AsyncIO `emit()`.
    '''

    @doc_inherit
    async def emit(self, data: Any):
        '''
        Emit signal data to connected slots.
        '''
        coroutines = [self._emit_to(handle, callback, data)
                      for handle, callback in self.slots.items()]
        asyncio.gather(*coroutines)
