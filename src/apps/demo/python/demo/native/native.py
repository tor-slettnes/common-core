#!/bin/echo Do not invoke directly.
#===============================================================================
## @file native.py
## @brief Demo - native Python implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from ..core import API, signal_store
from cc.core.scheduler import scheduler, TaskAlignment
from cc.protobuf.import_proto import import_proto
from cc.protobuf.wellknown import MessageToString, encodeTimestamp
from cc.protobuf.demo import encodeTimeStruct

### Standard Python modules
from typing import Callable
import time, sys, os.path

## Import symbols generated from `demo.proto`. These will appear in the namespace
## `cc.demo`
import_proto('demo', globals())

#===============================================================================
# Native Demo implementation

class NativeDemo (API):
    '''Demo API'''

    def __init__(self):
        API.__init__(self, 'Python Native')
        self.timer_task_handle = "Demo.ticker"

    def say_hello(self, greeting: cc.demo.Greeting):
        print("Emitting greeting: say_hello(%s)"%
              (MessageToString(greeting, as_one_line=True),))
        signal_store.emit_mapping('signal_greeting', None, greeting.identity, greeting)


    def get_current_time(self) -> cc.demo.TimeData:
        '''
        Get current time data.
        @return
            Current time data provided by the specific implementation.
        '''
        t = time.time()
        return cc.demo.TimeData(
            timestamp = encodeTimestamp(t),
            local_time = encodeTimeStruct(time.localtime(t)),
            utc_time = encodeTimeStruct(time.gmtime(t)),
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
