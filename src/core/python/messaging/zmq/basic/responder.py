#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file responder.py
## @brief ZeroMQ responder base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .endpoint import Endpoint
from cc.core.invocation import safe_invoke

### Third-party modules
import zmq

### Standard Python modules
import logging, threading

class Responder (Endpoint):
    endpoint_type = 'responder'

    def __init__(self,
                 bind_address: str,
                 channel_name: str|None = None,
                 project_name: str|None = None,
                 role: Endpoint.Role = Endpoint.Role.HOST,
                 ):

        Endpoint.__init__(
            self,
            address = bind_address,
            channel_name = channel_name,
            project_name = project_name,
            socket_type = zmq.REP,
            role = role)

        self.listen_thread = None
        self.keep_listening = False

    def start(self):
        self.initialize()
        self.keep_listening = True
        self.bind()
        t = self.listen_thread
        if not t or not t.is_alive():
            t = self.listen_thread = threading.Thread(
                None, self.run, 'Responder loop', daemon=True)
            t.start()

    def stop(self):
        try:
            self.keep_listening = False
            t = self.listen_thread
            if t and t.is_alive():
                logging.debug('Waiting for ZMQ responder thread')
                t.join()
        finally:
            self.unbind()
            self.deinitialize()

    def run(self):
        self.initialize()
        self.keep_listening = True
        try:
            self.bind()
            while self.keep_listening:
                request = self.receive_bytes()
                reply = self.process_request(request) or b''
                self.send_bytes(reply)
        finally:
            self.deinitialize()

    def process_request(self, request : bytes):
        logging.info("Received request: %r", request)
        return b''
