#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file grpc_base.py
## @brief Wrapper for gRPC-based services, common to client & server
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from ..common import Endpoint
from cc.core.settingsstore import SettingsStore

### Stanard Python modules
import re, logging
from typing import Optional

### Third-party modules
try:
    import grpc
except ImportError as e:
    logging.critical('Could not import module `grpc` - try installling `python3-grpcio`.')
    raise

#===============================================================================
# Base class

class Base (Endpoint):
    messaging_flavor = 'gRPC'

    # `service_name` should be overwritten by final subclass to look up settings
    # for this endpoint (e.g., host/port, ...).
    service_name = None

    def __init__ (self, service_name: str = None):
        if service_name is not None:
            self.service_name = service_name
        else:
            assert self.service_name is not None, \
                "Subclass %r should set `service_name` -- see %s"%\
                (type(self).__name__, __file__)

        Endpoint.__init__(self, self.service_name)

    def max_request_size(self) -> Optional[str]:
        return self.setting("max request size", None)

    def max_reply_size(self) -> Optional[str]:
        return self.setting("max reply size", None)


    def realaddress(self,
                     provided        : Optional[str],
                     hostOption      : str,
                     portOption      : int,
                     defaultHost     : str,
                     defaultPort     : int = 8080):
        '''Sanitize a service address of the form `[HOST][:PORT]` (where any or
        all components may be present) to the full form `HOST:PORT`.

        If either HOST or PORT is missing, defaults are determined as follows:

        * If the product-specific settings file
          `grpc-services-PRODUCT_NAME.json` contains a map entry for this gRPC
          service, the value is extracted from this map using the provided
          `hostOption` or `portOption` as key.

        * If still missing, the same lookup is performed in the file
          `grpc-services-common.json`.

        * Any attribute(s) that are still missing are populated from
          `defaultHost` or `defaultPort`, respectively.


        @param[in] provided
          Address to sanitize, normally provided as a command-line option.
        @param[in] hostOption
          Key to locate the host name in the settings file
        @param[in] portOption
          Key to locate the port number in the settings file
        @param[in] defaultHost
          Fallback if host name is not provided nor found in settings file
        @param[in] defaultPort
          Fallback if port number is not provided nor found in settings file
        @return
          Sanitized address of the form HOST:PORT (where HOST
          may still be empty)

        '''

        (host, port) = self._splitAddress(provided or "")

        if hostOption and not host:
            host = self.setting(hostOption, defaultHost)

        if portOption and port is None:
            port = self.setting(portOption, defaultPort)

        return self._joinAddress(host, port)


    _rx_address = re.compile(
        "(\\[[\\w\\.:]*\\]|[\\w\\.]*)"  # host, either '[x[:x...]]' or 'n[.n]...'
        "(?::(\\d+))?$"                 # port
    )

    def _splitAddress(self, target : str):
        if match := self._rx_address.match(target):
            host = match.group(1) or ""
            try:
                port = int(match.group(2))
            except (TypeError, ValueError):
                port = None

            return host, port
        else:
            return "", None

    def _joinAddress(self,
                     name: str,
                     port: int):

        if port is not None:
            return "%s:%s"%(name, port)
        else:
            return None

