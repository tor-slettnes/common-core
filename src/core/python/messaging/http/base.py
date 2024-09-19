#!/usr/bin/echo Do not invoke directly.
##===============================================================================n
## @file base.py
## @brief Common functionality for HTTP clients and servers
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from ..common import Endpoint
from ...core.invocation import check_type

### Standard Python modules
from typing import Optional
from collections import namedtuple

import http
import urllib.parse
import re

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

    def get_target_url(self, provided: str = ''):
        return urllib.parse.urlunsplit(self.get_target(provided))

    def get_target(self,
                   provided      : str = '',
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
          `http-endpoints-PRODUCT_NAME.json` contains a map entry for this
          service, the value is extracted from this map using the provided
          `schemeOption`, `hostOption`, `portOption` or `pathOption` as key.

        * If still missing, the same lookup is performed in the file
          `http-endpoints-common.json`.

        * Any attribute(s) that are still missing are populated from
          `defaultScheme`, `defaultHost`, `defaultPort`, and `defaultPath`,
          respectively.

        '''

        scheme, netloc, path, query, fragment = urllib.parse.urlsplit(provided or "")

        try:
            host, port = netloc.rsplit(':', 1)
        except ValueError:
            host, port = netloc, None

        if schemeOption and not scheme:
            scheme = self.setting(schemeOption, defaultScheme)

        if hostOption and not host:
            host = self.setting(hostOption, defaultHost)

        if portOption and port is None:
            port = self.setting(portOption, defaultPort)

        if pathOption and not path:
            path = self.setting(pathOption, defaultPath)

        netloc = f"{host}:{port}" if port else host

        return urllib.parse.SplitResult(scheme, netloc, path, query, fragment)


    def combined_url(self, base_url, rel_path, kwargs={}):
        return urllib.parse.urlunsplit(self.combined_part(base_url, rel_path, kwargs))


    def combined_parts(self, base_url, rel_path, kwargs={}):
        check_type(rel_path, str)
        check_type(kwargs, dict)

        scheme, netloc, base_path, base_query, fragment = urllib.parse.urlsplit(base_url)
        _, _,  rel_path, rel_query, _ = urllib.parse.urlsplit(rel_path)

        path  = os.path.join("/", base_path, rel_path)
        items = [q for q in (base_query, rel_query) if q]
        tuples = []
        for (key, arg) in kwargs.items():
            if isinstance(arg, (list, tuple)):
                tuples.extend([(key, value) for value in arg])
            else:
                tuples.append((key, arg))

        items.extend(["=".join((key, urllib.parse.quote_plus(str(value))))
                      for (key, value) in tuples])

        return urllib.parse.SplitResult(scheme, netloc, path, '&'.join(items), fragment)

