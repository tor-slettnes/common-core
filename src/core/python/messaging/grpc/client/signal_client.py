'''
Client base for gRPC services with a 'Signal' message type

Inherits `SignalMixIn` on top of `GenericClient`.
'''

### Common Core modules
from cc.protobuf.signal import SignalStore, SignalMessage

### Modules within package
from .signal_mixin import SignalMixIn
from .generic_client import GenericClient

#===============================================================================
# SignalClient

class SignalClient (SignalMixIn, GenericClient):
    '''
    gRPC client with additional functionality to receive streamed Signal
    messages from a gRPC server with a corresponding `Watch()` method.
    '''

    #===========================================================================
    # Instance methods

    def __init__(self,
                 host: str|None = None,
                 wait_for_ready: bool = False,
                 product_name: str|None = None,
                 project_name: str|None = None,
                 intercept_errors: bool = True,
                 signal_store: SignalStore|None = None,
                 signal_type: SignalMessage|None = None,
                 watch_all: bool = True,
                 use_cache: bool = True,
                 **kwargs):
        '''
        @param host
            Server host and/or port number, in the form `address:port`.
            `address` may be a hostname or an IPv4 or IPv6 address string.  If
            either address or host is missing, the default value is obtain from
            any of the following the settings file, in order:
            - grpc-endpoints-SERVICE_NAME.json,
            - grpc-endpoints-PRODUCT_NAME.json,
            - grpc-endpoints-PROJECT_NAME.json,
            - grpc-endpoints-common.json.
           (The ALL CAPS portions are substituted as appropriate)

        @param wait_for_ready
            If a connection attempt fails, keep retrying until successful.
            This value may be overriden per call.

        @param product_name
            Name of the product, used to locate corresponding settings files
            (e.g. `grpc-endpoints-PRODUCT.yaml`).

        @param project_name
            Name of code project (e.g. parent code repository). Used to locate
            corresponding settings files (e.g., `grpc-endpoints-PROJECT.yaml`)

        @param interceptor_errors
            Raise any errors encountered in custom gRPC interceptors

        @param signal_store
            Use an existing `SignalStore()` instance instead of creating a new
            one.  This can be useful if signals are (received and) emitted from
            both this client and other parts of your code, for instance other
            messaging endpoints.  This argument may also be provided as a class
            attribute.  If both are missing, `signal_type` must be provided.

        @param signal_type
            If `signal_store` is not provided, create a new `SignalStore` or
            `CachedSignalStore` instance (depending on the `use_cache` input)
            using this ProtoBuf message type as its signal type.  This argument
            may also be provided as a class attribute.

        @param use_cache
            If creating a new `SignalStore` instance (i.e. if `signal_store` is
            not provided), use the derived `CachedSignalStore` type. This
            retains the most recent data value of each signal received from the
            server.  If the signal includes a `key` field (i.e., if it is a
            `MappingSignal` instance), keep the most recent data value per key.
            These values can later be queried using `get_cached_map()`.

        @param watch_all
            Watch all signals (specify an empty filter to server), even if
            not connected to slots. This is useful in order to populate the
            local signal cache, which can later be queried. A side effect
            is that the watching thread automatically starts once instantiated.
        '''

        GenericClient.__init__(
            self,
            host = host,
            wait_for_ready = wait_for_ready,
            product_name = product_name,
            project_name = project_name,
            intercept_errors = intercept_errors)

        SignalMixIn.__init__(
            self,
            signal_store = signal_store,
            signal_type = signal_type,
            watch_all = watch_all,
            use_cache = use_cache,
        )

    def initialize(self):
        GenericClient.initialize(self)
        SignalMixIn.initialize(self)

    def deinitialize(self):
        SignalMixIn.deinitialize(self)
        GenericClient.deinitialize(self)
