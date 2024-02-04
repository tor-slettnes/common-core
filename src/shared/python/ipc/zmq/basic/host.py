#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file host.py
## @brief ZeroMQ host/listener base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .endpoint import Endpoint

### Third-party modules
import zmq

### Standard Python modules
import logging

class Host (Endpoint):
    def __init__(self,
                 address     : str,
                 channel_name: str,
                 socket_type : zmq.SocketType):

        Endpoint.__init__(self, channel_name, socket_type)
        self.bound = False
        self.address = self._realaddress(
            address,
            'scheme', 'host', 'port',
            'tcp', '*', 5555)

    def initialize(self):
        Endpoint.initialize(self)
        self.bind()

    def deinitialize(self):
        self.unbind()
        Endpoint.deinitialize(self)

    def bind(self):
        if not self.bound:
            logging.info("Binding %s to %s"%(self, self.address))
            self.socket.bind(self.address)
            self.bound = True

    def unbind(self):
        if self.bound:
            logging.info("Unbinding %s from %s"%(self, self.address))
            self.socket.unbind(self.address)
            self.bound = False
