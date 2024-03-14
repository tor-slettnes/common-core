#!/usr/bin/echo Do not invoke directly.
##===============================================================================n
## @file client.py
## @brief gRPC client base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .base import Base
from .status import DetailedError
from cc.io.protobuf import ProtoBuf, CC
from cc.core.invocation import invocation

### Third-party modules
import grpc

### Standard Python modules
import sys
import logging

class Client (Base):
    '''gRPC client wrapper with miscellaneous convenience features.

    By deriving from this class, dedicated service clients will automatically
    connect to the correct server address (host and port) as specified in the
    corresponding service settings.  Additionally, call wrappers are provided to
    extract custom error details in case of failures.

    '''

    ## Messaging endpoint type
    endpoint_type = 'client'

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

    def __init__ (self,
                  host: str,
                  wait_for_ready: bool = False,
                  use_asyncio: bool = False):
        '''Initializer.  Parameters:

        @param[in] host
            Server host and/or port number, in the form `address:port`.
            `address` may be a hostname or an IPv4 or IPv6 address string.
            If either address or host is missing, the default value is
            obtain from the service settings file (grpc-endpoints-PRODUCT.json).

        @param[in] wait_for_ready
            If a connection attempt fails, keep retrying until successful.
            This value may be overriden per call.

        @param[in] asyncio
            Use Python AsyncIO.  Effectively this performs calls within a
            `grpc.aio.Channel` instance, rather than the default `grpc.Channel`.
            Additionally, the `call()` method uses AsyncIO semantics to capture
            any exceptions.

        '''

        Base.__init__(self, self.service_name or self._default_service_name())

        assert type(self).Stub, \
            "Subclass should set 'Stub' to appropriate gRPC service class"

        self.wait_for_ready = wait_for_ready
        self.use_asyncio    = use_asyncio
        self.lastError      = None
        self.host           = self.realaddress(host, "host", "port", "localhost", 8080)
        self._init_channel()

    def _init_channel(self):
        logging.debug("Creating gRPC service channel %r to %s"%
                      (self.channel_name, self.host))

        if self.use_asyncio:
            self.channel = grpc.aio.insecure_channel(self.host)
            self.invoke  = self._invoke_async
        else:
            self.channel = grpc.insecure_channel(self.host)
            self.invoke  = self._invoke_direct

        self.stub = type(self).Stub(self.channel)
        # self.channel.subscribe(self._channelChange)

    def _default_service_name(self):
        service_name, _ = type(self).Stub.__name__.rsplit('Stub')
        return service_name

    def call(self, method, request=ProtoBuf.Empty(), wait_for_ready=None, **kwargs):
        if wait_for_ready is None:
            wait_for_ready = self.wait_for_ready

        return self.invoke(method, request, wait_for_ready=wait_for_ready, **kwargs)

    def stream_from(self, method, request=ProtoBuf.Empty(), wait_for_ready=True, **kwargs):
        if wait_for_ready is None:
            wait_for_ready = self.wait_for_ready

        return self._invoke_direct(method, request, wait_for_ready=wait_for_ready, **kwargs)

    def _invoke_direct(self, method, request=ProtoBuf.Empty(), print_failure=True, **kwargs):
        try:
            return method(request, **kwargs)
        except grpc.RpcError as e:
            self._capture_error(e, sys.exc_info(), print_failure)

    async def _invoke_async(self, method, request=ProtoBuf.Empty(), print_failure=True, **kwargs):
        try:
            return await method(request, **kwargs)
        except grpc.aio.AioRpcError as e:
            self._capture_error(e, sys.exc_info(), print_failure)

    def _capture_error(self,
                       error,
                       exc_info,
                       print_failure):
        e_type, e_name, e_tb = exc_info
        self.lastError = error = DetailedError(e.with_traceback(e_tb))
        if print_failure:
            print("=== While invoking gRPC method %s(%s):\n--> %s\n"%(
                method._method.decode(),
                ProtoBuf.MessageToString(request, as_one_line=True),
                str(error).strip().replace("\n", "\n... ")))
        raise error from None

    def _channelChange (self, *args, **kwargs):
        logging.info("%s: Channel change callback: %s"%
                     (__file__,
                      invocation(self._channelChange, args, kwargs)))

