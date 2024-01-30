#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file satellite.py
## @brief ZeroMQ satellite/connect base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import re, argparse, zmq
from .endpoint import Endpoint, ArgParser as BaseParser

class Satellite (Endpoint):
    def __init__(self,
                 address     : str,
                 channel_name: str,
                 socket_type : zmq.SocketType):

        Endpoint.__init__(self, channel_name, socket_type)

        self.address = self._realaddress(
            address,
            'scheme', 'host', 'port',
            'tcp', 'localhost', 5555)

    def initialize(self):
        self.connect()

    def deinitialize(self):
        pass

    def connect(self):
        self.socket.connect(self.address)

class ArgParser(BaseParser):
    def __init__ (self, *args, **kwargs):
        BaseParser.__init__(self, *args, **kwags)
        self.add_argument('--host', type=str,
                          help='Host address, in the form [SCHEME://][HOST][:PORT]')

