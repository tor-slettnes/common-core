#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file requester.py
## @brief ZeroMQ requester base
## @author Tor Slettnes
#===============================================================================

### Modules within package
from .endpoint import Endpoint

### Third-party modules
import zmq

class Requester (Endpoint):
    endpoint_type = 'requester'
    socket_type = zmq.REQ

    def __init__(self,
                 address: str,
                 channel_name: str|None = None,
                 product_name: str|None = None,
                 project_name: str|None = None,
                 role: Endpoint.Role = Endpoint.Role.SATELLITE):

        Endpoint.__init__(**locals())

    def send_receive(self, request : bytes) -> bytes:
        self.send_bytes(request)
        return self.receive_bytes()
