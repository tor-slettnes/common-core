#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file base.py
## @brief Wrapper for gRPC-based services, common to client & server
## @author Tor Slettnes
#===============================================================================

### Modules within package
from ..common import Endpoint
from ...core.settingsstore import SettingsStore

### Stanard Python modules
import re
import logging
import os
import urllib.parse
from collections import namedtuple
from typing import Optional

### Third-party modules
try:
    ### Workaround for gRPC segfault with `"fork"` setting of `aiomultiprocess` module
    # os.environ["GRPC_ENABLE_FORK_SUPPORT"] = "false"
    import grpc
except ImportError as e:
    logging.critical('Could not import module `grpc` - try installling `python3-grpcio`.')
    raise

#-------------------------------------------------------------------------------
# Type declarations

AddressPair = namedtuple('AddressPair', ('host', 'port'))

#-------------------------------------------------------------------------------
# Base class

class Base (Endpoint):
    messaging_flavor = 'gRPC'

    # `service_name` should be overwritten by final subclass to look up settings
    # for this endpoint (e.g., host/port, ...).
    service_name = None

    def __init__(self,
                 service_name: str|None = None,
                 product_name: str|None = None,
                 project_name: str|None = None,
                 ):

        if service_name is not None:
            self.service_name = service_name
        else:
            assert self.service_name is not None, \
                "Subclass %r should set `service_name` -- see %s"%\
                (type(self).__name__, __file__)

        Endpoint.__init__(self,
                          channel_name = self.service_name,
                          product_name = product_name,
                          project_name = project_name)


    def max_request_size(self) -> Optional[str]:
        return self.setting("max request size", None)

    def max_reply_size(self) -> Optional[str]:
        return self.setting("max reply size", None)

    def peer(self, context: grpc.RpcContext) -> str:
        return urllib.parse.unquote(context.peer())


    def sanitize_address(self,
                         host       : str|None,
                         port       : str|int|None,
                         hostOption : str,
                         portOption : int,
                         defaultHost: str,
                         defaultPort: int = 8080) -> AddressPair:
        '''
        Sanitize a host string and/or port number to a valid (host, port)
        tuple.

        The `host` input may optionally include the port number, in the form
        `HOST:PORT`. However, if a `port` input is also provided explicitly,
        that takes precedence.

        If either host or port is missing, defaults are determined as follows:

        * If the product-specific settings file
          `grpc-endpoints-PRODUCT_NAME.json` contains a map entry for this gRPC
          service, the value is extracted from this map using the provided
          `hostOption` or `portOption` as key.

        * If still missing, the same lookup is performed in the file
          `grpc-endpoints-common.json`.

        * Any attribute(s) that are still missing are populated from
          `defaultHost` or `defaultPort`, respectively.

        @param host
          Host address to sanitize, normally provided as a command-line option.

        @param port
          Explicit port number, if any.

        @param hostOption
          Key to locate the host name in the settings file

        @param portOption
          Key to locate the port number in the settings file

        @param defaultHost
          Fallback if host name is not provided nor found in settings file

        @param defaultPort
          Fallback if port number is not provided nor found in settings file

        @returns
          A `(host, port)` tuple, where `host` is a string and `port` is an
          integer.
        '''

        (host, port) = self._splitAddress(host or "", port)

        if hostOption and not host:
            host = self.setting(hostOption, defaultHost)

        if portOption and port is None:
            port = self.setting(portOption, defaultPort)

        return AddressPair(host, port)


    _rx_address = re.compile(
        "(\\[[\\w\\.:]*\\]|[\\w\\-\\.]*)"  # host, either '[x[:x...]]' or 'n[.n]...'
        "(?::(\\d+))?$"                    # port
    )

    def _splitAddress(self,
                      target: str,
                      port: int|None = None):

        if m := self._rx_address.match(target):
            host = m.group(1) or ""
            if port is None and m.group(2):
                port = int(m.group(2))

            return host, port
        else:
            return "", port

    def _joinAddress(self, pair: AddressPair):

        if pair.port is not None:
            return "%s:%s"%pair
        else:
            return None

