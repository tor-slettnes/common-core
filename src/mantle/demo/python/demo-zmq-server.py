#!/usr/bin/python3 -i
#===============================================================================
## @file demo-zmq-server.py
## @brief Python server for `Demo` ZMQ RPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from demo.zmq.server import Server
from demo.native     import NativeDemo

### Standard Python modules
import logging

if __name__ == '__main__':
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)

    demoserver = Server(NativeDemo(), "")
    logging.info("Starting Python ZMQ Demo Server")
    demoserver.initialize()
