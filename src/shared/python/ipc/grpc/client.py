#!/usr/bin/echo Do not invoke directly.
##===============================================================================n
## @file client.py
## @brief Interactive wrapper for access to Instrument Services via gRPC
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import sys
import logging
import traceback
import grpc
import inspect
import argparse

from .base     import Base, ProtoBuf, CC
from .status   import DetailedError

#===============================================================================
## Enumerated values

class ArgParser (argparse.ArgumentParser):
    def __init__ (self, host=None, *args, **kwargs):
        super().__init__(prog=sys.argv[0], *args, **kwargs)
        self.add_argument(
            '--host',
            type=str,
            default=host,
            help='Server address, in the form [PERSONALITY@][HOST][:PORT].')


class Client (Base):
    stub = None

    def __init__ (self, host, wait_for_ready=False):
        super().__init__()

        assert type(self).stub, \
            "Subclass should set 'stub' to appropriate gRPC service class"

        self.host           = self._realaddress(host, "host", "port", "localhost", 8080)
        self.channel        = grpc.insecure_channel(self.host)
        #self.channel.subscribe(self._channelChange)
        self.stub           = type(self).stub(self.channel)
        self.wait_for_ready = wait_for_ready
        self.wait_warned    = False
        self.lastError      = None

    def detailedError (self, e, exc_info):
        e_type, e_name, e_tb = exc_info
        self.lastError = error = DetailedError(e.with_traceback(e_tb))
        return error

    def _invoke (self, method, request=ProtoBuf.Empty(), print_failure=True, **kwargs):
        try:
            return method(request, **kwargs)
        except grpc.RpcError as e:
            error = self.detailedError(e, sys.exc_info());
            if print_failure:
                print("=== While invoking gRPC method %s(%s):\n--> %s\n"%(
                    method._method.decode(),
                    ProtoBuf.formatMesssage(request),
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

        return  self._invoke(method, request, **kwargs)

    def _wrapstream (self, method, request=ProtoBuf.Empty(), **kwargs):
        response = []
        response.extend(self._invoke(method, request, **kwargs) or ())
        return response

    def _channelChange (self, *args, **kwargs):
        logging.info("%s: Channel change callback: %s"%
                     (__file__,
                      self._invocation(self._channelChange, args, kwargs)))

