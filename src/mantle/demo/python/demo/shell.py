#!/usr/bin/python3 -i
#===============================================================================
## @file shell.py
## @brief Interactive service control via collection of clients.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.demo.grpc.client import DemoClient as gRPCDemoClient
from cc.demo.zmq.client import DemoClient as ZMQDemoClient

import cc.protobuf.demo
import cc.protobuf.wellknown
import cc.protobuf.variant
import cc.protobuf.signal
import cc.protobuf.status
import cc.protobuf.rr

import cc.core.invocation

### Third-party modules
import google.protobuf.message

### Standard Python modules
import logging
import argparse
import sys
import os.path

### Add a few arguments to the base argparser
class ArgParser (argparse.ArgumentParser):
    def __init__ (self, prog=os.path.basename(sys.argv[0]), identity="DemoShell", *args, **kwargs):
        super().__init__(prog=prog, *args, **kwargs);

        self.add_argument('--identity',
                          type=str,
                          default=identity,
                          help="Identity of this client")

        self.add_argument('--host',
                          type=str,
                          default="localhost",
                          help="Host for remote services")

        self.add_argument('--wait-for-ready',
                          action='store_const',
                          default=False,
                          const=True,
                          help="Wait until server is ready instead of failing immediately")

        self.add_argument('--debug',
                          action='store_const',
                          default=False,
                          const=True,
                          help='Print debug messages')

def foo():
    cc.core.invocation.caller_frame()


def legend():
    '''
    Interactive Service Control.  Subsystems loaded:

        demo_grpc - Simple gRPC communications example
        demo_zmq - Simple 0MQ communications example

    Generated ProtoBuf data types and associated wrapper methods are generally
    available in the `cc.protobuf` namespace, e.g.:

      - cc.protobuf.demo      - Data types for the Demo service
      - cc.protobuf.wellknown - Well-known types from Google

    Use 'help(subsystem)' to list available subcomponents or methods
    '''
    print(legend.__doc__)


if __name__ == '__main__':
    args   = ArgParser().parse_args()

    pylogger = logging.getLogger()
    pylogger.setLevel((logging.INFO, logging.DEBUG)[args.debug])

    demo_grpc = gRPCDemoClient(
        args.host,
        identity = args.identity,
        wait_for_ready = args.wait_for_ready)

    demo_zmq  = ZMQDemoClient(
        args.host,
        identity = args.identity)

    demo_grpc.initialize()
    demo_zmq.initialize()
    legend()
