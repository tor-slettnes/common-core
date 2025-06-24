#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file requester.py
## @brief ZeroMQ requester base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .endpoint import Endpoint

### Third-party modules
import zmq

class Requester (Endpoint):
    endpoint_type = 'requester'

    def __init__(self,
                 host_address: str,
                 channel_name: str,
                 project_name: str|None = None,
                 role: Endpoint.Role = Endpoint.Role.SATELLITE):

        Endpoint.__init__(
            self,
            address = host_address,
            channel_name = channel_name,
            project_name = project_name,
            socket_type = zmq.REQ,
            role = role)


    def send_receive(self, request : bytes) -> bytes:
        self.send_bytes(request)
        return self.receive_bytes()
