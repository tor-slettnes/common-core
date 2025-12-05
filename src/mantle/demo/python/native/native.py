#!/bin/echo Do not invoke directly.
#===============================================================================
## @file native.py
## @brief Demo - native Python implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from ..base import API, demo_signals
from cc.core.scheduler import scheduler, TaskAlignment
from cc.protobuf.wellknown import MessageToString, encodeTimestamp
from cc.protobuf.datetime import encodeTimeStruct
from cc.demo.protobuf import Greeting, TimeData

### Standard Python modules
from typing import Callable
import time, sys, os.path

#===============================================================================
# Native Demo implementation

class NativeDemo (API):
    '''
    Demo API
    '''

    def __init__(self):
        API.__init__(self, 'Python Native')
        self.timer_task_handle = "Demo.ticker"

    def say_hello(self, greeting: Greeting):
        demo_signals.emit_mapping(
            signal_name = 'signal_greeting',
            key = greeting.identity,
            value = greeting)


    def get_current_time(self) -> TimeData:
        '''
        Get current time data.

        @returns
            Current time data provided by the specific implementation.
        '''
        t = time.time()
        return TimeData(
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
        demo_signals.emit_event('signal_time', self.get_current_time())
