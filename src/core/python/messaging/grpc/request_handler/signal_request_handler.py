'''
signal_request_handler.py - gRPC request handler with `watch` method to feed back signals
'''

__docformat__ = 'javadoc en'
__author__= 'Tor Slettnes'

### Standard Python modules
from typing import Optional, Generator
from queue  import Queue
import queue, logging

### Third-party modules
import google.protobuf.message
import grpc

### Common Core modules
from cc.protobuf.signal import SignalStore, Filter, MappingAction

### Modules within package
from .generic_request_handler import RequestHandler

#===============================================================================
# @class SignalRequestHandler

class SignalRequestHandler (RequestHandler):
    '''
    gRPC request handler with `Watch()` method to stream signals back to client.
    '''

    signal_store = None

    def __init__(self,
                 signal_store   : SignalStore|None = None,
                 bind_address   : str = "",
                 max_queue_size : Optional[int] = 256,
                 product_name   : str|None = None,
                 project_name   : str|None = None,
                 ):
        '''
        param[in] signal_store
           An instance of `protobuf.signal.SignalStore` from which
           we receive signals.

        param[in] bind_address
           Listener address, which will be added to gRPC server.  For additional
           information, refer to `__init__()` in the `RequestHandler` parent class.

        param[in] max_queue_size
           Max size of queue that will hold locally-emitted signals until
           they are streamed back to the client.  None means unlimited.

        @param product_name
            Name of the product, used to locate corresponding settings files
            (e.g. `grpc-endpoints-PRODUCT.yaml`).

        @param project_name
            Name of code project (e.g. parent code repository). Used to locate
            corresponding settings files (e.g., `grpc-endpoints-PROJECT.yaml`)
        '''

        if signal_store:
            self.signal_store = signal_store

        else:
            assert self.signal_store, (
                'SignalRequestHandler subclass %s must define or pass in `signal_store`' % (
                    type(self).__name__,
                ))


        RequestHandler.__init__ (self,
                                 bind_address = bind_address,
                                 product_name = product_name,
                                 project_name = project_name)

        self.max_queue_size = max_queue_size


    def Watch(self,
              request: Filter,
              context: grpc.ServicerContext
              ) -> Generator[None, google.protobuf.message.Message, None]:
        '''
        Invoked by gRPC client to stream signal events back to client.

        One invoked, we create a `Queue` instance which will capture
        signals emitted locally on the server side, then yield messages
        from this queue back to the client.
        '''

        queue = Queue(self.max_queue_size)
        self.last_context = context

        try:
            for signal_name in self.signal_store.signal_names(request):
                self.signal_store.connect_signal(signal_name, queue.put_nowait)

            ### Add an empty message to indicate the end of the initial cache
            queue.put_nowait(self.signal_store.signal_type())

            while msg := queue.get():
                yield msg

        finally:
            self.signal_store.disconnect_signal(signal_name)
