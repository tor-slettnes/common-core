#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file zmq_base.py
## @brief Wrapper for ZeroMQ-based applications
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import sys, re, socket, logging, argparse

from buildinfo     import PROJECT_NAME
from settingsstore import SettingsStore

#===============================================================================
# Base class

class Channel (object):
    # `product_name` is used to lookup product-specific information,
    # and may be overridden in subclasses.
    product_name = PROJECT_NAME

    # `channel_name` should be overwritten by final subclass to look up settings
    # for this communications channel (e.g., protocol/host/port, ...)
    channel_name = None

    def __init__(self, channel_name: str = None):

        if channel_name:
            self.channel_name = channel_name
        else:
            assert self.channel_name, \
                "Subclass %r should set 'channel_name' -- see %s"%\
                (type(self).__name__, __file__)

    @property
    def settings(self):
        return NotImplementedError # Override in protocol-specific subclass


    def setting(self,
                key         : str,
                personality : str,
                default     : str = None) -> object:

        try:
            return self.settings["personalities"][personality][key]
        except KeyError:
            try:
                return self.settings["defaults"][self.channel_name][key]
            except KeyError:
                return default


    def _realaddress(self,
                     provided        : str,
                     protocolOption  : str,
                     hostOption      : str,
                     portOption      : int,
                     defaultProtocol : str = "tcp",
                     defaultHost     : str = "",
                     defaultPort     : int = 5555):
        '''
        @param[in] address
          Address to sanitize, normally provided as a command-line option.
        @param[in] protocolOption
          Key to locate the protocol name in the settings file
        @param[in] hostOption
          Key to locate the host name in the settings file
        @param[in] portOption
          Key to locate the port number in the settings file
        @param[in] defaultProtocol
          Fallback if protocol name isn ot provided nor found in setitngs file
        @param[in] defaultHost
          Fallback if host name is not provided nor found in settings file
        @param[in] defaultPort
          Fallback if port number is not provided nor found in settings file
        @return
          Sanitized address of the form PROTOCOL://HOST:PORT (where HOST
          may still be empty)

        Sanitize a service address of the form
        `[PROTOCOL://][PERSONALITY@][HOST][:PORT]` (where any or all components
        may be present) to the full form `PROTOCOL://HOST:PORT`.

        If either PROTOCOL, HOST or PORT is missing, defaults are determined as
        follows:

        * If PERSONALITY is provided, but either the product-specific settings
          file "zmq-channel-PRODUCT_NAME.json" or the common settings file
          "zmq-channel-common.json" contains a top-level "personalities" map
          with the corresponding PERSONALITY branch, the value is extracted
          from that branch.

        * If the value is still missing, but either of the above settings files
          contain a "defaults" map with a CHANNEL_NAME branch, the value is
          extracted from that branch.

        * In either case, the value is extracted from the resulting branch
          using the corresponding lookup key passed in as `protocolOption`,
          `hostOption` or `portOption`.

        * Any attribute(s) that are still missing are populated from
          `defaultProtocol`, `defaultHost` or `defaultPort`, respectively.

        See the provided settings file `zmq-channels-common.json` for more
        details on settings syntax.
        '''

        (protocol, personality, host, port) = self._splitAddress(provided or "")

        if protocolOption and not protocol:
            protocol = self.setting(protocolOption, personality, defaultProtocol)

        if hostOption and not host:
            host = self.setting(hostOption, personality, defaultHost)

        if portOption and port is None:
            port = self.setting(portOption, personality, defaultPort)

        return self._joinAddress(protocol, host, port)


    _rx_address = re.compile(
        "(?:(\\w*)://)?"                # protocol
        "(?:(\\w*)@)?"                  # personality
        "(\\[[\\w\\.:]*\\]|[\\w\\.]*)"  # host, either '[x[:x...]]' or 'n[.n]...'
        "(?::(\\d+))?$"                 # port
    )

    def _splitAddress(self,
                      target : str):

        match = self._rx_address.match(target)
        if match:
            protocol = match.group(1) or ""
            personality = match.group(2) or ""
            host = match.group(3) or ""
            try:
                port = int(match.group(4))
            except (TypeError, ValueError):
                port = None

            return protocol, personality, host, port
        else:
            return "", "", "", None


    def _joinAddress(self,
                     protocol : str,
                     name     : str,
                     port     : int):
        raise NotImplementedError # Override in protocol-specific subclass


class ArgParser(argparse.ArgumentParser):
    def __init__ (self, *args, **kwargs):
        argparse.ArgumentParser.__init__(self, prog=sys.argv[0], *args, **kwargs)
