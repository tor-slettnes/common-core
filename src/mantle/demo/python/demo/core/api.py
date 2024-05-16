#!/bin/echo Do not invoke directly.
#===============================================================================
## @file demo.py
## @brief Demo - native Python implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from .signals import signal_store
import protobuf.demo
import protobuf.variant
import protobuf.wellknown

### Standard Python modules
from typing import Callable
import sys, os.path, time

SignalSlot = Callable[[protobuf.demo.Signal], None]

#===============================================================================
# Native Demo implementation

class API:
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
        '''Say hello to anyone who might be listening.

        @param[in] text
            What we're telling our listeners
        @param[in] kwargs
            Arbitrary attributes included in greeting
        @note
            This is a convencience wrapper to build a `protobuf.demo.Greeting` object
            and pass it to `say_hello()`.
        '''

        greeting = protobuf.demo.Greeting(
            text = text,
            identity = self.identity,
            implementation = self.implementation,
            birth = protobuf.wellknown.encodeTimestamp(self.birth),
            data = protobuf.variant.encodeValueList(kwargs))

        return self.say_hello(greeting)


    def say_hello(self, greeting: protobuf.demo.Greeting):
        '''Issue a greeting to anyone who may be listening.
        For interactive use, the `hello()` wrapper method may be more convenient.

        @param[in] greeting
            A greeting for our listeners.
        '''

        raise NotImplementedError("Method not implemented by %s"%(self,))


    def get_current_time(self) -> protobuf.demo.TimeData:
        '''Get current time data.

        @return
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
        '''Register a callback to be invoked whenever a greeting is received

        @param[in] callback
            Callback method, which will receive the `protobuf.demo.Signal()`
            message containing the greeting as argument.
        '''
        signal_store.connect_signal('signal_greeting', callback)


    def stop_notify_greetings(self):
        '''
        Unregister any existing greeting callback
        '''
        signal_store.disconnect_signal('signal_greeting')


    def start_notify_time(self, callback: SignalSlot):
        '''
        @brief Register a callback to be invoked whenver the time updated (1x/second)

        @param[in] callback
            Callback method, which will receive the time data as argument.
        '''
        signal_store.connect_signal_data('signal_time', callback)


    def stop_notify_time (self):
        '''
        @brief Unregister any existing time callbacks
        '''

        signal_store.disconnect_signal('signal_time')
