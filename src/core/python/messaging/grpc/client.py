#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief gRPC client base
## @author Tor Slettnes
#===============================================================================

### Modules within package
from .base import Base, AddressPair
from .client_interceptor import ClientInterceptor, AsyncClientInterceptor
from cc.core.invocation import invocation

### Third-party modules
import grpc

### Standard Python modules
import sys
import logging
import os

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

    ## To use Python AsyncIO semantics, subclasses should set `use_asyncio` flag.
    use_asyncio = False

    def __init__(self,
                 host: str|None = None,
                 wait_for_ready: bool = False,
                 use_asyncio: bool|None = None,
                 intercept_errors: bool = True,
                 product_name: str|None = None,
                 project_name: str|None = None,
                 ):
        '''
        Initializer.  Parameters:

        @param host
            Server host and/or port number, in the form `address:port`.
            `address` may be a hostname or an IPv4 or IPv6 address string.  If
            either address or host is missing, the default value is obtain from
            the service settings file (grpc-endpoints-PRODUCT.json).

        @param wait_for_ready
            If a connection attempt fails, keep retrying until successful.
            This value may be overriden per call.

        @param use_asyncio
            Use Python AsyncIO.  Effectively this performs calls within a
            `grpc.aio.Channel` instance, rather than the default `grpc.Channel`.
            Additionally, the `call()` method uses AsyncIO semantics to capture
            any exceptions.  If not specified, the default value is obtained
            from the corresponding `use_asyncio` class variable.

        @param interceptor_errors
            Raise any errors encountered in custom gRPC interceptors

        @param product_name
            Name of the product, used to locate corresponding settings files
            (e.g. `grpc-endpoints-PRODUCT.yaml`).

        @param project_name
            Name of code project (e.g. parent code repository). Used to locate
            corresponding settings files (e.g., `grpc-endpoints-PROJECT.yaml`)
        '''

        Base.__init__(self,
                      service_name = self.service_name or self._default_service_name(),
                      product_name = product_name,
                      project_name = project_name)

        assert type(self).Stub, (
            "gRPC Client subclass %r should set 'Stub' to appropriate gRPC service class"%
            (type(self).__name__,))

        if use_asyncio is not None:
            self.use_asyncio = use_asyncio

        self.wait_for_ready   = wait_for_ready
        self.intercept_errors = intercept_errors
        self.service_address  = self.realaddress(host, "host", "port", "localhost", 8080)
        self.channel          = self.create_channel()
        self.stub             = self.Stub(self.channel)

        #self.channel.subscribe(self._channelChange)

    @property
    def host(self) -> str:
        return self._joinAddress(self.service_address)

    def create_channel(self):
        if self.use_asyncio:
            # self._channel = grpc.aio.insecure_channel(target = self.host)
            return grpc.aio.insecure_channel(
                target = self.host,
                interceptors = [
                    AsyncClientInterceptor(self.wait_for_ready,
                                           self.intercept_errors)
                ])

        else:
            # self._channel = grpc.insecure_channel(self.host)
            return grpc.intercept_channel(
                grpc.insecure_channel(self.host),
                ClientInterceptor(self.wait_for_ready,
                                  self.intercept_errors))

    def close(self):
        return self.channel.close()

    def _default_service_name(self):
        service_name, _ = type(self).Stub.__name__.rsplit('Stub')
        return service_name

    def _channelChange (self, *args, **kwargs):
        self.logger.debug("gRPC channel change: args=%s, kwargs=%s"%(args, kwargs))

