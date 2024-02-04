#!/bin/echo Do not invoke directly.
#===============================================================================
## @file demo.py
## @brief Demo - native Python implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from ..core          import API, CC, ProtoBuf, demo_signals
from core.scheduler  import scheduler, ALIGN_UTC
from core.invocation import safe_invoke

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

    def say_hello(self, greeting: CC.Demo.Greeting):
        print("Emitting greeting: say_hello(%s)"%(
            ProtoBuf.MessageToString(greeting, as_one_line=True),))
        demo_signals.emit_mapping('greeting', None, greeting.identity, greeting)


    def get_current_time(self) -> CC.Demo.TimeData:
        '''
        Get current time data.
        @return
            Current time data provided by the specific implementation.
        '''
        t = time.time()
        return CC.Demo.TimeData(
            timestamp = ProtoBuf.encodeTimestamp(t),
            local_time = CC.encodeTimeStruct(time.localtime(t)),
            utc_time = CC.encodeTimeStruct(time.gmtime(t)),
        )


    def start_ticking(self) -> None:
        '''
        Tell the server to start issuing periodic time updates
        '''
        scheduler.schedule(self.timer_task_handle, # handle
                           1.0,                    # interval
                           self._emit_time,        # method
                           align = ALIGN_UTC)          # time alignment


    def stop_ticking(self) -> None:
        '''
        Tell the server to stop issuing periodic time updates
        '''
        scheduler.unschedule(self.timer_task_handle)

    def _emit_time(self):
        t = time.time()
        demo_signals.emit_event(
            'time',
            CC.Demo.TimeData(
                timestamp = ProtoBuf.encodeTimestamp(t),
                local_time = CC.encodeTimeStruct(time.localtime(t)),
                utc_time = CC.encodeTimeStruct(time.gmtime(t)),
            ))
