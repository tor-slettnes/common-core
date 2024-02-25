#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file responder.py
## @brief ZeroMQ responder base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .host           import Host
from cc.core.invocation import safe_invoke

### Third-party modules
import zmq

### Standard Python modules
import logging, threading

class Responder (Host):
    endpoint_type = 'responder'

    def __init__(self,
                 bind_address: str,
                 channel_name: str = None):

        Host.__init__(self, bind_address, channel_name, zmq.REP)
        self.listen_thread = None
        self.keep_listening = False

    def initialize(self):
        Host.initialize(self)
        self.start()

    def deinitialize(self):
        self.stop()
        Host.deinitialize(self)

    def start(self):
        self.keep_listening = True
        self.bind()
        t = self.listen_thread
        if not t or not t.is_alive():
            t = self.listen_thread = threading.Thread(
                None, self.run, 'Responder loop', daemon=True)
            t.start()

    def stop(self):
        self.keep_listening = False
        t = self.listen_thread
        if t and t.is_alive():
            logging.debug('Waiting for ZMQ responder thread')
            t.join()
        self.unbind()

    def run(self):
        self.keep_listening = True
        self.bind()
        while self.keep_listening:
            request = self.receive_bytes()
            reply = self.process_request(request) or b''
            self.send_bytes(reply)


    def process_request(self, request : bytes):
        logging.info("Received request: %r", request)
        return b''
