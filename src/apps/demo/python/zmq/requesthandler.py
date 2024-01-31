#!/usr/bin/python3 -i
#===============================================================================
## @file demo_zmq_requesthandler.py
## @brief Python request handler for `Demo` ZMQ RPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ..common import CC, ProtoBuf, Weekdays, DEMO_RPC_INTERFACE
from ipc.zmq.protobuf.requesthandler import RequestHandler

import logging

class DemoRequestHandler (RequestHandler):

    def __init__(self):
        RequestHandler.__init__(self, DEMO_RPC_INTERFACE)

    def say_hello(self, request: CC.Demo.Greeting):
        logging.info("Received hello(%s)"%(ProtoBuf.MessageToString(request),))

    def get_current_time(self) -> CC.Demo.TimeData:
        logging.info("Received get_current_time()")

    def start_ticking(self):
        logging.info("Received start_ticking()")

    def stop_ticking(self):
        logging.info("Received stop_ticking()")
