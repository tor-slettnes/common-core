#!/bin/echo Do not invoke directly.
#===============================================================================
## @file api.py
## @brief Demo - native Python implementation
## @author Tor Slettnes
#===============================================================================

### Standard Python modules
from typing import Callable
import sys, os.path, time

### Modules within package
from cc.protobuf.wellknown import encodeTimestamp
from cc.protobuf.variant import encodeKeyValueMap
from cc.core.logbase import LogBase

### Modules relative to install dir
from .signals import demo_signals
from ..protobuf import Greeting, TimeData, Signal

PROJECT_NAME = "demo"

SignalSlot = Callable[[Signal], None]

#===============================================================================
# Native Demo implementation

class API (LogBase):
    '''Demo API'''

    def __init__(self,
                 implementation : str,
                 identity       : str = None):

        self.implementation = implementation or str(self)
        self.birth          = time.time()
        self.identity       = identity or os.path.basename(sys.argv[0])

    def hello(self,
              text: str,
              **kwargs):
        '''
        Say hello to anyone who might be listening.

        @param text:
            What we're telling our listeners
        @param kwargs:
            Arbitrary attributes included in greeting

        @note
            This is a wrapper function to build a `cc.protobuf.demo.Greeting`
            object and pass it to `say_hello()`.
        '''

        greeting = Greeting(
            text = text,
            identity = self.identity,
            implementation = self.implementation,
            birth = encodeTimestamp(self.birth),
            data = encodeKeyValueMap(kwargs))

        return self.say_hello(greeting)


    def say_hello(self, greeting: Greeting):
        '''
        Issue a greeting to anyone who may be listening.  For interactive
        use, the `hello()` wrapper method may be more convenient.

        @param greeting:
            A greeting for our listeners.
        '''

        raise NotImplementedError("Method not implemented by %s"%(self,))


    def get_current_time(self) -> TimeData:
        '''
        Get current time data.

        @returns
            Current time data provided by the specific implementation.
        '''

        raise NotImplementedError("Method not implemented by %s"%(self,))


    def start_ticking(self) -> None:
        '''
        Tell the server to start issuing periodic time updates
        '''
        raise NotImplementedError("Method not implemented by %s"%(self,))


    def stop_ticking(self) -> None:
        '''
        Tell the server to stop issuing periodic time updates
        '''
        raise NotImplementedError("Method not implemented by %s"%(self,))


    def start_notify_greetings(self, callback: SignalSlot):
        '''
        Register a callback to be invoked whenever a greeting is received

        @param callback:
            Callback method, which will receive `cc.protobuf.demo.Signal()`
            messages containing the greeting as its sole argument.
        '''
        demo_signals.connect_signal('signal_greeting', callback)


    def stop_notify_greetings(self):
        '''
        Unregister any existing greeting callback
        '''
        demo_signals.disconnect_signal('signal_greeting')


    def start_notify_time(self, callback: SignalSlot):
        '''
        Register a callback to be invoked whenver the time updated
        (1x/second)

        @param callback:
            Callback method, which will receive the time data as argument.
        '''
        demo_signals.connect_signal_data('signal_time', callback)


    def stop_notify_time (self):
        '''
        Unregister any existing time callbacks
        '''

        demo_signals.disconnect_signal('signal_time')
