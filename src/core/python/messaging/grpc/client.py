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
                 use_asyncio: bool = False,
                 project_name: str|None = None,
                 ):
        '''
        Initializer.  Parameters:

        @param host
            Server host and/or port number, in the form `address:port`.
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
        self._stub          = None
        self._stub_owner    = None

    @property
    def stub(self):
        '''
        Return the current gRPC client stub.

        A new stub is created under one of these conditions:
          - One has not yet been created
          - Our PID has changed since we last obtained this reference,
            meaning we have forked and are now a child of the parent
            process that originally created this stub.

        '''
        pid = os.getpid()
        if (self._stub == None) or (self._stub_owner != pid):
            self.create_channel()
            self._stub_owner = pid

        return self._stub

    @stub.deleter
    def stub(self):
        self._stub = None
        self.channel = None


    def create_channel(self, host: str|None = None):
        '''
        Establish communication to the server.

        @param host
            Override the server host and/or port number previously provided to
            `__init__()`, if any.
        '''

        if host is None:
            host = self.host

        self._init_channel(host)


    def delete_channel(self):
        '''
        Disconnect from the server instance.
        '''
        self._stub = None
        self.channel = None

    def _init_channel(self, host: str):
        logging.debug("Creating gRPC service channel %r to %s"%
                      (self.channel_name, host))

        if self.use_asyncio:
            self.channel = grpc.aio.insecure_channel(
                target = host,
                interceptors = [AsyncClientInterceptor(self.wait_for_ready)])
            # self.channel = grpc.aio.insecure_channel(target = host)

        else:
            # self.channel = grpc.insecure_channel(host)
            self.channel = grpc.intercept_channel(
                grpc.insecure_channel(host),
                ClientInterceptor(self.wait_for_ready))

        self._stub = type(self).Stub(self.channel)
        self.host = host
        # self.channel.subscribe(self._channelChange)

    def _default_service_name(self):
        service_name, _ = type(self).Stub.__name__.rsplit('Stub')
        return service_name

    def _channelChange (self, *args, **kwargs):
        logging.debug("Channel change: args=%s, kwargs=%s"%(args, kwargs))

