#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file requester.py
## @brief ZeroMQ requester base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .satellite   import Satellite

### Third-party modules
import zmq

class Requester (Satellite):
    endpoint_type = 'requester'

    def __init__(self,
                 host_address: str,
                 channel_name: str):

        Satellite.__init__(self, host_address, channel_name, zmq.REQ)


    def send_receive(self, request : bytes) -> bytes:
        self.send_bytes(request)
        return self.receive_bytes()
