#!/bin/echo Do not invoke directly.
#===============================================================================
## @file endpoint.py
## @brief Wrapper for ZeroMQ-based applications
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from ...common.endpoint import Endpoint as EndpointBase

### Standard Python modules
import re, sys, logging

### Third-party modules
try:
    import zmq
except ImportError as e:
    logging.critical('Could not import module `zmq` - try installing `python3-zmq`.')
    raise e from None


class Endpoint (EndpointBase):
    messaging_flavor  = 'ZMQ'
    context = zmq.Context()

    def __init__(self,
                 channel_name: str,
                 project_name: str|None,
                 socket_type : zmq.SocketType,
                 ):

        EndpointBase.__init__(self,
                              channel_name = channel_name,
                              project_name = project_name)
        self.socket = self.context.socket(socket_type)

    def send_bytes(self,
                   data:  bytes,
                   flags: zmq.Flag = 0):
        self.socket.send(data, flags=flags)

    def receive_bytes(self,
                      flags: zmq.Flag = 0) -> bytes:
        data = b''
        more = True

        while more:
            msg   = self.socket.recv(flags, copy=False)
            data += msg.buffer
            more  = msg.more

            logging.debug('Received size=%d bytes, total=%d bytes, more=%s: %r'%(
                len(msg.buffer),
                len(data),
                msg.more,
                msg.buffer))

        return data


    def _realaddress(self,
                     provided      : str,
                     schemeOption  : str,
                     hostOption    : str,
                     portOption    : int,
                     defaultScheme : str = "tcp",
                     defaultHost   : str = "",
                     defaultPort   : int = 5555):
        '''
        Sanitize a service address of the form `[SCHEME://][HOST][:PORT]`
        (where any or all components may be present) to the full form
        `SCHEME://HOST:PORT`.

        If either SCHEME, HOST or PORT is missing, defaults are determined as
        follows:

        * If the product-specific settings file
          `zmq-endpoints-PROJECT_NAME.json` contains a map entry for this ZMQ
          channel name, the value is extracted from this map using the provided
          `schemeOption`, `hostOption` or `portOption` as key.

        * If still missing, the same lookup is performed in the file
          `zmq-endpoints-common.json`.

        * Any attribute(s) that are still missing are populated from
          `defaultScheme`, `defaultHost` or `defaultPort`, respectively.

        @param address:
          Address to sanitize, normally provided as a command-line option.
        @param schemeOption:
          Key to locate the scheme in the settings file
        @param hostOption:
          Key to locate the host name in the settings file
        @param portOption:
          Key to locate the port number in the settings file
        @param defaultScheme:
          Fallback if scheme name is not provided nor found in settings file
        @param defaultHost:
          Fallback if host name is not provided nor found in settings file
        @param defaultPort:
          Fallback if port number is not provided nor found in settings file
        @returns
          Sanitized address of the form SCHEME://HOST:PORT (where HOST
          may still be empty)
        '''

        (scheme, host, port) = self._splitAddress(provided or "")

        if schemeOption and not scheme:
            scheme = self.setting(schemeOption, defaultScheme)

        if hostOption and not host:
            host = self.setting(hostOption, defaultHost)

        if portOption and port is None:
            port = self.setting(portOption, defaultPort)

        return self._joinAddress(scheme, host, port)


    _rx_address = re.compile(
        "(?:(\\w*)://)?"                       # scheme
        "(\\*|\\[[\\w\\.:]*\\]|[\\w\\-\\.]*)"  # host, either '[x[:x...]]' or 'n[.n]...'
        "(?::(\\d+))?"                         # port
    )

    def _splitAddress(self,
                      target : str):

        if match := self._rx_address.match(target):
            scheme = match.group(1) or ""
            host = match.group(2) or ""
            try:
                port = int(match.group(3))
            except (TypeError, ValueError):
                port = None

            return scheme, host, port
        else:
            return "", "", None

    def _joinAddress(self,
                     scheme : str,
                     name   : str,
                     port   : int):

        return "".join([
            f"{scheme}://" if scheme else "",
            name,
            f":{port}" if port else ""
        ])
