'''
signal_request_handler.py - gRPC request handler with `watch` method to feed back signals
'''

__docformat__ = 'javadoc en'
__author__= 'Tor Slettnes'

### Common Core modules
from cc.protobuf.signal import SignalStore

### Modules within package
from .signal_mixin import SignalMixIn
from .generic_request_handler import RequestHandler

#===============================================================================
# @class SignalRequestHandler

class SignalRequestHandler (SignalMixIn, RequestHandler):
    '''
    gRPC request handler with `Watch()` method to stream signals back to client.
    '''

    def __init__(self,
                 signal_store   : SignalStore|None = None,
                 bind_address   : str = "",
                 max_queue_size : int|None = 256,
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

        RequestHandler.__init__ (self,
                                 bind_address = bind_address,
                                 product_name = product_name,
                                 project_name = project_name)

        SignalMixIn.__init__(self,
                             signal_store = signal_store,
                             max_queue_size = max_queue_size)
