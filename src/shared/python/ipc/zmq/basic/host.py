#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file host.py
## @brief ZeroMQ host/listener base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import re, argparse, zmq
from .endpoint import Endpoint, ArgParser as BaseParser

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
        self.bind()

    def deinitialize(self):
        pass

    def bind(self):
        if not self.bound:
            self.socket.bind(self.address)
            self.bound = True

    def unbind(self):
        if self.bound:
            self.socket.unbind(self.address)
            self.bound = False

class ArgParser(BaseParser):
    def __init__ (self, *args, **kwargs):
        BaseParser.__init__(self, *args, **kwags)
        self.add_argument('--bind', type=str,
                          help='Host address, in the form [SCHEME://][HOST][:PORT]')

