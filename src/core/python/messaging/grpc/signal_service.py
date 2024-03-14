#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file signalservice.py
## @brief gRPC service handler with `watch` method to feed back signals
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from .service  import Service
from cc.io.protobuf import CC, ProtoBuf, SignalStore

### Third-party modules
import grpc

### Standard Python modules
import queue, logging
from typing import Optional, Generator
from queue  import Queue

#===============================================================================
# @class SignalService

class SignalService (Service):
    '''
    gRPC Servicer with `watch()` method to stream signals back to client.
    '''

    def __init__ (self,
                  signal_store   : SignalStore,
                  bind_address   : str = "",
                  max_queue_size : Optional[int] = 256):
        '''
        param[in] signal_store
           An instance of `messaging.base.signalstore.SignalStore`, from which
           we receive signals.

        param[in] bind_address
           Listener address, which will be added to gRPC server.

        param[in] max_queue_size
           Max size of queue that will hold locally-emitted signals until
           they are streamed back to the client.  None means unlimited.
        '''

        Service.__init__ (self, bind_address)
        self.signal_store = signal_store
        self.max_queue_size = max_queue_size


    def watch(self,
              request: CC.Signal.Filter,
              context: grpc.ServicerContext
              ) -> Generator[None, ProtoBuf.Message, None]:
        '''
        Invoked by gRPC client to stream signal events back to client.

        One invoked, we create a `Queue` instance which will capture
        signals emitted locally on the server side, then yield messages
        from this queue back to the client.
        '''

        queue = Queue(self.max_queue_size)
        self.last_context = context

        try:
            signals = self.signal_list(request)
            for signal_name in self.signal_list(request):
                self.signal_store.connect_signal(signal_name, queue.put_nowait)

            while msg := queue.get():
                yield msg

        finally:
            self.signal_store.disconnect_signal(signal_name)


    def signal_list(self, signal_filter: CC.Signal.Filter):
        polarity = bool(signal_filter.polarity)
        requested = set(signal_filter.indices)

        return [f_name
                for f_name, f_number in self.signal_store.signal_fields().items()
                if polarity == (f_number in requested)]
