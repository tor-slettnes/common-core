'''
generic_client.py - Generic gRPC client base, for making service calls through
a standard gRPC channel.
'''

__author__ = "Tor Slettnes"
__docformat__ = "javadoc en"

### Standard Python modules
from collections.abc import abstractmethod

### Third-party modules
import grpc

### Modules within package
from ..base import Base
from .interceptors import GenericClientInterceptor
from .readers import AbstractReader, ThreadReader

class GenericClient (Base):
    '''
    Abstract base class for gRPC service clients.

    By deriving from this class, dedicated service clients will automatically
    connect to the correct server address (host and port) as specified in the
    corresponding service settings.  Additionally, call wrappers are provided to
    extract custom error details in case of failures.

    Final implementations should not derive from this class directly,
    but rather from `Client` or `AsyncClient`.
    '''

    ## Messaging endpoint type, used by Endpoint base class
    endpoint_type = 'client'

    ## Used to look up service address in settings
    host_option = 'host'

    ## Subclasses should set `Stub` to the client stub class imported from
    ## the generated `_pb2_grpc.py` module, e.g.:
    ## ```
    ##     from my_service_pb2_grpc import MyServiceStub as Stub
    ## ```
    Stub = None

    ## `service_name` is optional. If not provided by the subclass it is
    ## determined from the above stub.  It is used to look up service specific
    ## settings, such as target host/port.
    service_name = None

    def __init__(self,
                 host: str|None = None,
                 wait_for_ready: bool = False,
                 product_name: str|None = None,
                 project_name: str|None = None,
                 intercept_errors: bool = True,
                 ):
        '''
        Initializer.  Parameters:

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

        '''

        Base.__init__(self,
                      service_name = self.service_name or self._default_service_name(),
                      product_name = product_name,
                      project_name = project_name)

        assert type(self).Stub, (
            "gRPC Client subclass %r should set 'Stub' to an appropriate gRPC client stub class" % (
                type(self).__name__,
            ))

        self.wait_for_ready   = wait_for_ready
        self.intercept_errors = intercept_errors
        self.service_address  = self.realaddress(host, "host", "port", "localhost", 8080)
        self.channel          = self.create_channel()
        self.stub             = self.Stub(self.channel)

        #self.channel.subscribe(self._channelChange)

    @property
    def host(self) -> str:
        return self._joinAddress(self.service_address)

    def create_channel(self) -> grpc.Channel:
        # self._channel = grpc.insecure_channel(self.host)
        return grpc.intercept_channel(
            grpc.insecure_channel(self.host),
            GenericClientInterceptor(self.wait_for_ready,
                                     self.intercept_errors))

    def create_reader(self) -> AbstractReader:
        return ThreadReader()

    def close(self):
        return self.channel.close()

    def _default_service_name(self):
        service_name, _ = type(self).Stub.__name__.rsplit('Stub')
        return service_name

    def _channelChange (self, *args, **kwargs):
        self.logger.debug("gRPC channel change: args=%s, kwargs=%s"%(
            args,
            kwargs,
        ))

