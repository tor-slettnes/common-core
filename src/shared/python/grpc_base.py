#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file grpc_base.py
## @brief Wrapper for gRPC-based services, common to client & server
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import sys, re, socket, logging, argparse

from settingsstore           import SettingsStore
from protobuf_standard_types import ProtoBuf
from protobuf_custom_types   import CC

#===============================================================================
# ServiceBase class

class ServiceBase (object):
    _settingsFiles = ("common-services.json", "product-services.json")

    servicename = None
    settings = None

    def __init__ (self, servicename=None):
        if servicename:
            self.servicename = servicename
        else:
            assert self.servicename, \
                "Subclass %s should set 'servicename' to appropriate gRPC service name"%\
                (type(self).__name__)

        if self.settings is None:
            ServiceBase.settings = SettingsStore(self._settingsFiles)

    def getServiceSetting (self, key, personality, default=None):
        try:
            return self.settings["personalities"][personality][key]
        except KeyError:
            try:
                return self.settings["defaults"][self.servicename][key]
            except KeyError:
                return default


    def _realaddress (self, provided, hostOption, portOption,
                      defaultHost="", defaultPort=8080):

        (personality, host, port) = self._splitAddress(provided or "")
        if not host:
            host = self.getServiceSetting(hostOption, personality, defaultHost)

        if port is None:
            port = self.getServiceSetting(portOption, personality)

        if port is None:
            try:
                port = socket.getservbyname(self.servicename)
            except socket.error as e:
                port = defaultPort
                logging.warning(("No port number specified, and no %r service exists; "
                                 "using default address %s:%d")%
                                (self.servicename, host, port))

        return "%s:%s"%(host, port)


    _rx_address = re.compile(
        "(?:(\\w*)@)?"                  # personality
        "(\\[[\\w\\.:]*\\]|[\\w\\.]*)"  # host, either '[x[:x...]]' or 'n[.n]...'
        "(?::(\\d+))?$"                 # port
    )

    def _splitAddress (self, target):
        match = self._rx_address.match(target)
        if match:
            personality = match.group(1) or ""
            host = match.group(2) or ""
            try:
                port = int(match.group(3))
            except (TypeError, ValueError):
                port = None

            return personality, host, port
        else:
            return "", "", None


class ArgParser(argparse.ArgumentParser):
    def __init__ (self, *args, **kwargs):
        argparse.ArgumentParser.__init__(self, prog=sys.argv[0], *args, **kwargs)
