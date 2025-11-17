#!/bin/echo Do not invoke directly.
#===============================================================================
## @file endpoint.py
## @brief Wrapper for ZeroMQ-based applications
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Standard Python modules
import enum
import logging
import re
import sys

### Third-party modules
try:
    import zmq
except ImportError as e:
    self.logger.critical('Could not import module `zmq` - try installing `pyzmq`.')
    raise e from None

### Modules within package
from ...common.endpoint import Endpoint as EndpointBase


class Endpoint (EndpointBase):
    messaging_flavor  = 'ZMQ'
    context = zmq.Context()

    class Role (enum.IntEnum):
        UNDEFINED = 0
        HOST      = 1
        SATELLITE = 2

    def __init__(self,
                 address     : str,
                 channel_name: str,
                 product_name: str|None,
                 socket_type : zmq.SocketType,
                 role        : Role = Role.UNDEFINED,
                 ):
        '''
        Initializer.

        @param address
            Depending on `role`, this should be a network interface address to
            which to bind (use `*` to denote all network interfaces), or a host
            address to which to connect.

        @param channel_name
            A name used to look up settings such as default host/interface
            address and port number for this endpoint.  Think of it as a
            generalized service name, adapted for RPC/Request/Response, Pub/Sub,
            and other ZMQ socket types.

        @param product_name
            Name of the overall code project, used to locate corresponding
            settings files (e.g. `zmq-endpoints-PROJECT.yaml`), in addition to
            `"common"`.
            (Since this is Python, it must be provided as a runtime argument
            rather than a pre-built default. Correspondingly, the meaing of
            "project" in this context is flexible; for instance, if your code
            repository is shared amongst multiple products, it could denote
            a specific one.)

        @param socket_type
            ZMQ socket type to use for this endpoint. Typically a hardcoded
            value is passed in from direct descendants of this class, e.g.
            `Publisher` would use `zmq.PUB`.

        @param role
            Default role of this endpoint vis a vis its peers.
            `Endpoint.Role.HOST` means that this will bind to one or all local
            network interfaces and listen for incoming connections, whereas
            `Endpoint.Role.SATELLITE` means that this will connect to a (local
am            or remote) peer.  If not provided, an explicit call to `bind()` or
            `connect()` will be required in order to communicate with peers.
        '''

        EndpointBase.__init__(self,
                              channel_name = channel_name,
                              product_name = product_name)

        self._provided_address = address
        self.socket            = self.context.socket(socket_type)
        self.role              = role
        self.bound_address     = None
        self.host_address      = None

    def initialize(self):
        '''
        Initialize this endpoint.
        '''
        super().initialize()

        if self.role == Endpoint.Role.HOST:
            self.bind()
        elif self.role == Endpoint.Role.SATELLITE:
            self.connect()


    def deinitialize(self):
        '''
        Deinitialize this endpoint.
        '''

        if self.role == Endpoint.Role.HOST:
            self.unbind()
        elif self.role == Endpoint.Role.SATELLITE:
            self.disconnect()

        super().deinitialize()


    def send_bytes(self,
                   data:  bytes,
                   flags: zmq.Flag = 0):
        '''
        Send raw bytes over this instance's ZMQ socket.
        '''

        self.socket.send(data, flags=flags)


    def receive_parts(self,
                      flags: zmq.Flag = 0) -> list[bytes]:
        '''
        Read one ZMQ message from this instance's ZMQ socket as individual
        parts.
        '''

        data = []
        more = True

        while more:
            msg  = self.socket.recv(flags, copy=False)
            data.append(msg.buffer)
            more = msg.more

        return data

    def receive_bytes(self,
                      flags: zmq.Flag = 0) -> bytes:
        '''
        Read one ZMQ message from this instance's ZMQ socket with parts
        compbined into a single bytes object.
        '''

        return b''.join(self.receive_parts(flags))

    def sanitized_host_address(self,
                               provided: str|None = None) -> str:
        '''
        Sanitize the provided host address into a format as expected by ZMQ.

        @param provided
            Address to sanitize, if different from what was previously provided
            to `__init__()`.  Missing components (`scheme`, `host`, `port`) are
            populated with default values: `tcp`, `localhost`, `5555`.

        @return
            Sanitized address of the form `scheme://host:port`.
        '''

        return self._realaddress(provided,
                                 "scheme", "host", "port",
                                 "tcp", "localhost", 5555)

    def connect(self, address: str|None = None):
        '''
        Connect to another ZMQ endpoint.

        @param address
            Address of ZMQ endpoint.  See `sanitized_host_address()` for details.
        '''

        if not self.host_address:
            host_address = self.sanitized_host_address(address)
            self.logger.debug("%s connecting to %s"%(self, host_address))
            self.socket.connect(host_address)
            self.host_address = host_address

    def disconnect(self):
        '''
        Disconnect from any connected ZMQ endpoint
        '''

        if host_address := self.host_address:
            self.host_address = None
            self.logger.info("%s disconnecting from %s"%(self, host_address))
            self.socket.disconnect(host_address)

    def sanitized_bind_address(self,
                               provided: str|None = None) -> str:
        '''
        Sanitize the provided bind (interface) address into a format as expected by ZMQ.

        @param provided
            Address to sanitize, if different from what was previously provided
            to `__init__()`.  Missing components (`scheme`, `host`, `port`) are
            populated with default values: `tcp`, `*`, `5555`.

        @return
            Sanitized address of the form `scheme://host:port`.
        '''

        return self._realaddress(provided,
                                 "scheme", "listen", "port",
                                 "tcp", "*", 5555)

    def bind(self, address: str|None = None):
        '''
        Bind this instance's ZMQ socket to one or all local network
        interface(s) to listen for incoming connections.

        @param address
            Interface address, or `*` to bind to all interfaces.
            See `sanitized_bind_address()` for details.
        '''

        if not self.bound_address:
            bind_address = self.sanitized_bind_address(address)
            self.logger.info("%s binding to %s"%(self, bind_address))
            self.socket.bind(bind_address)
            self.bound_address = bind_address


    def unbind(self):
        '''
        Unbind this instance's ZMQ socket from any network interface.
        '''

        if bound_address := self.bound_address:
            self.bound_address = None
            self.logger.info("%s unbinding from %s"%(self, bound_address))
            self.socket.unbind(bound_address)


    def _realaddress(self,
                     provided      : str|None = None,
                     schemeOption  : str = "scheme",
                     hostOption    : str = "",
                     portOption    : int = "port",
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
          `zmq-endpoints-PRODUCT_NAME.json` contains a map entry for this ZMQ
          channel name, the value is extracted from this map using the provided
          `schemeOption`, `hostOption` or `portOption` as key.

        * If still missing, the same lookup is performed in the file
          `zmq-endpoints-common.json`.

        * Any attribute(s) that are still missing are populated from
          `defaultScheme`, `defaultHost` or `defaultPort`, respectively.

        @param address
          Address to sanitize, normally provided as a command-line option.

        @param schemeOption
          Key to locate the scheme in the settings file

        @param hostOption
          Key to locate the host name in the settings file

        @param portOption
          Key to locate the port number in the settings file

        @param defaultScheme
          Fallback if scheme name is not provided nor found in settings file

        @param defaultHost
          Fallback if host name is not provided nor found in settings file

        @param defaultPort
          Fallback if port number is not provided nor found in settings file

        @returns
          Sanitized address of the form SCHEME://HOST:PORT (where HOST
          may still be empty)
        '''

        (scheme, host, port) = self._splitAddress(provided or self._provided_address or "")

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
                     scheme : str|None,
                     name   : str,
                     port   : int|None):

        return "".join([
            f"{scheme}://" if scheme else "",
            name,
            f":{port}" if port else ""
        ])
