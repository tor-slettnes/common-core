#!/usr/bin/echo Do not invoke directly.
##===============================================================================n
## @file client.py
## @brief Interactive wrapper for access to Instrument Services via gRPC
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from core.invocation import invocation
from .base import Base
from .status import DetailedError
from ..protobuf import ProtoBuf, CC

### Third-party modules
import grpc

### Standard Python modules
import sys
import logging
import traceback
import inspect

class Client (Base):
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

    def __init__ (self, host, wait_for_ready=False):
        Base.__init__(self, self.service_name or self._service_name())

        assert type(self).Stub, \
            "Subclass should set 'Stub' to appropriate gRPC service class"

        self.host           = self.realaddress(host, "host", "port", "localhost", 8080)
        logging.info("Creating gRPC service %r channel to %s"%(self.service_name, self.host))
        self.channel        = grpc.insecure_channel(self.host)
        # self.channel.subscribe(self._channelChange)
        self.stub           = type(self).Stub(self.channel)
        self.wait_for_ready = wait_for_ready
        self.wait_warned    = False
        self.lastError      = None

    def detailedError (self, e, exc_info):
        e_type, e_name, e_tb = exc_info
        self.lastError = error = DetailedError(e.with_traceback(e_tb))
        return error

    def _service_name(self):
        service_name, _ = type(self).Stub.__name__.rsplit('Stub')
        return service_name

    def _invoke (self, method, request=ProtoBuf.Empty(), print_failure=True, **kwargs):
        try:
            return method(request, **kwargs)

        except grpc.RpcError as e:
            error = self.detailedError(e, sys.exc_info());
            if print_failure:
                print("=== While invoking gRPC method %s(%s):\n--> %s\n"%(
                    method._method.decode(),
                    ProtoBuf.MessageToString(request, as_one_line=True),
                    str(error).strip().replace("\n", "\n... ")))
            raise error from None

    def _wrap (self, method, request=ProtoBuf.Empty(), wait_for_ready=None, **kwargs):
        if wait_for_ready is None:
            wait_for_ready = self.wait_for_ready

        if 'wait_for_ready' in inspect.signature(method).parameters:
            kwargs.update(wait_for_ready=wait_for_ready)

        elif wait_for_ready and not self.wait_warned:
            logging.warning("Python gRPC library version does not support 'wait_for_ready' flag; "
                            "please upgrade your system")
            self.wait_warned = True

        return self._invoke(method, request, **kwargs)

    def _wrapstream (self, method, request=ProtoBuf.Empty(), **kwargs):
        response = []
        response.extend(self._invoke(method, request, **kwargs) or ())
        return response

    def _channelChange (self, *args, **kwargs):
        logging.info("%s: Channel change callback: %s"%
                     (__file__,
                      invocation(self._channelChange, args, kwargs)))

