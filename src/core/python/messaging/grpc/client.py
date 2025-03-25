#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief gRPC client base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .base import Base
from .client_interceptor import ClientInterceptor, AsyncClientInterceptor
from cc.core.invocation import invocation

### Third-party modules
import grpc

### Standard Python modules
import sys
import logging

class Client (Base):
    '''
    gRPC client wrapper with miscellaneous convenience features.

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

    def __init__(self,
                 host: str,
                 wait_for_ready: bool = False,
                 use_asyncio: bool = False,
                 project_name: str|None = None,
                 ):
        '''
        Initializer.  Parameters:

        @param host Server host and/or port number, in the form `address:port`.
            `address` may be a hostname or an IPv4 or IPv6 address string.  If
            either address or host is missing, the default value is obtain from
            the service settings file (grpc-endpoints-PRODUCT.json).

        @param wait_for_ready:
            If a connection attempt fails, keep retrying until successful.
            This value may be overriden per call.

        @param use_asyncio:
            Use Python AsyncIO.  Effectively this performs calls within a
            `grpc.aio.Channel` instance, rather than the default `grpc.Channel`.
            Additionally, the `call()` method uses AsyncIO semantics to capture
            any exceptions.
        '''

        Base.__init__(self,
                      service_name = self.service_name or self._default_service_name(),
                      project_name = project_name)

        assert type(self).Stub, \
            "Subclass should set 'Stub' to appropriate gRPC service class"

        self.wait_for_ready = wait_for_ready
        self.use_asyncio    = use_asyncio
        self.host           = self.realaddress(host, "host", "port", "localhost", 8080)
        self._init_channel()

    def _init_channel(self):
        logging.debug("Creating gRPC service channel %r to %s"%
                      (self.channel_name, self.host))

        if self.use_asyncio:
            self.channel = grpc.aio.insecure_channel(
                target = self.host,
                interceptors = [AsyncClientInterceptor(self.wait_for_ready)])
            # self.channel = grpc.aio.insecure_channel(target = self.host)

        else:
            # self.channel = grpc.insecure_channel(self.host)
            self.channel = grpc.intercept_channel(
                grpc.insecure_channel(self.host),
                ClientInterceptor(self.wait_for_ready))

        self.stub = type(self).Stub(self.channel)
        # self.channel.subscribe(self._channelChange)

    def _default_service_name(self):
        service_name, _ = type(self).Stub.__name__.rsplit('Stub')
        return service_name

    def _channelChange (self, *args, **kwargs):
        logging.debug("Channel change: args=%s, kwargs=%s"%(args, kwargs))

