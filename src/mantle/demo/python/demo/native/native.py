#!/bin/echo Do not invoke directly.
#===============================================================================
## @file demo.py
## @brief Demo - native Python implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from ..core import API, signal_store
from core.scheduler import scheduler, TaskAlignment
import protobuf.demo
import protobuf.wellknown

### Standard Python modules
from typing import Callable
import time, sys, os.path


#===============================================================================
# Native Demo implementation

class NativeDemo (API):
    '''Demo API'''

    def __init__(self):
        API.__init__(self, 'Python Native')
        self.timer_task_handle = "Demo.ticker"

    def say_hello(self, greeting: protobuf.demo.Greeting):
        print("Emitting greeting: say_hello(%s)"%
              (protobuf.wellknown.MessageToString(greeting, as_one_line=True),))
        signal_store.emit_mapping('signal_greeting', None, greeting.identity, greeting)


    def get_current_time(self) -> protobuf.demo.TimeData:
        '''
        Get current time data.
        @return
            Current time data provided by the specific implementation.
        '''
        t = time.time()
        return protobuf.demo.TimeData(
            timestamp = protobuf.wellknown.encodeTimestamp(t),
            local_time = protobuf.demo.encodeTimeStruct(time.localtime(t)),
            utc_time = protobuf.demo.encodeTimeStruct(time.gmtime(t)),
        )


    def start_ticking(self) -> None:
        '''
        Tell the server to start issuing periodic time updates
        '''
        scheduler.add(self.timer_task_handle, # handle
                      1.0,                    # interval
                      self._emit_time,        # method
                      align = TaskAlignment.UTC) # time alignment


    def stop_ticking(self) -> None:
        '''
        Tell the server to stop issuing periodic time updates
        '''
        scheduler.remove(self.timer_task_handle)

    def _emit_time(self):
        t = time.time()
        signal_store.emit_event('signal_time', self.get_current_time())
