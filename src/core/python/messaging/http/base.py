#!/usr/bin/echo Do not invoke directly.
##===============================================================================n
## @file base.py
## @brief Common functionality for HTTP clients and servers
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from ..common import Endpoint
from typing import Optional
from collections import namedtuple

import re

HTTPTarget = namedtuple('HTTPTarget', ['scheme', 'host', 'port', 'path'])

class HTTPError (RuntimeError):
    pass

class HTTPClientError (HTTPError):
    pass

class HTTPServerError (HTTPError):
    pass


class HTTPBase (Endpoint):
    messaging_flavor = 'HTTP'

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


    def get_target(self,
                   provided      : Optional[str],
                   schemeOption  : str = "scheme",
                   hostOption    : str = "host",
                   portOption    : int = "port",
                   pathOption    : str = "path",
                   defaultScheme : str = "http",
                   defaultHost   : str = "localhost",
                   defaultPort   : int = None,
                   defaultPath   : str = ""):
        '''Sanitize a service address of the form
        `[SCHEME://][HOST][:PORT][/LOCATION]` (where any or all components may
        be present) to the full form `SCHEME://HOST:PORT/LOCATION`.

        If any component is missing, defaults are determined as follows:

        * If the product-specific settings file
          `grpc-services-PRODUCT_NAME.json` contains a map entry for this
          service, the value is extracted from this map using the provided
          `schemeOption`, `hostOption`, `portOption` or `pathOption` as key.

        * If still missing, the same lookup is performed in the file
          `grpc-services-common.json`.

        * Any attribute(s) that are still missing are populated from
          `defaultScheme`, `defaultHost`, `defaultPort`, and `defaultPath`,
          respectively.

        '''

        (scheme, host, port, path) = self.split_url(provided or "")

        if schemeOption and not scheme:
            scheme = self.setting(schemeOption, defaultScheme)

        if hostOption and not host:
            host = self.setting(hostOption, defaultHost)

        if portOption and port is None:
            port = self.setting(portOption, defaultPort)

        if pathOption and not path:
            path = self.setting(pathOption, defaultPath)

        return HTTPTarget(scheme, host, port, path)


    _rx_address = re.compile(
        "(?:(\\w+)://)?"                      # scheme
        "(\\[[0-9A-Fa-f:]+\\]|[\\w\\-\\.]+)"  # `[ip6::address]` or `f.q.d.n`
        "(?::(\\d+))?"                        # port number
        "(/.*)?$"                             # path
    )

    def split_url(self, target : str):
        if match := self._rx_address.match(target):
            scheme = match.group(1) or ""
            host   = match.group(2) or ""
            try:
                port = int(match.group(3))
            except (TypeError, ValueError):
                port = None
            path = match.group(4) or ""
            return scheme, host, port, path
        else:
            return "", "", None, ""

    def join_url(self,
                 scheme: str,
                 host  : str,
                 port  : int,
                 path  : str):

        return "".join([
            f"{scheme}://" if scheme else "",
            host,
            f":{port}" if port else "",
            path
        ])


