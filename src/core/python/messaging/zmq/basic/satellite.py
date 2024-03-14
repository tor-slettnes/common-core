#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file satellite.py
## @brief ZeroMQ satellite/connect base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .endpoint import Endpoint

### Third-party modules
import zmq

### Sstandard python modules
import logging

class Satellite (Endpoint):
    def __init__(self,
                 address     : str,
                 channel_name: str,
                 socket_type : zmq.SocketType):

        Endpoint.__init__(self, channel_name, socket_type)

        self.connected = False
        self.address = self._realaddress(
            address,
            'scheme', 'host', 'port',
            'tcp', 'localhost', 5555)

    def initialize(self):
        Endpoint.initialize(self)
        self.connect()

    def deinitialize(self):
        Endpoint.deinitialize(self)

    def connect(self):
        if not self.connected:
            logging.debug("Connecting %s to %s"%(self, self.address))
            self.socket.connect(self.address)
            self.connected = True

    def disconnect(self):
        if self.connected:
            logging.debug("Disconnecting %s from %s"%(self, self.address))
            self.socket.disconnect(self.address)
            self.connected = False
