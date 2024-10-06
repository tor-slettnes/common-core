#!/usr/bin/python3 -i
#===============================================================================
## @file demoshell.py
## @brief Interactive service control via collection of clients.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.demo.grpc.client import DemoClient as gRPCDemoClient
from cc.demo.zmq.client import DemoClient as ZMQDemoClient

from cc.protobuf.import_proto \
    import import_wellknown_protos, import_core_protos, import_proto

import cc.protobuf.demo
import cc.protobuf.wellknown
import cc.protobuf.variant
import cc.protobuf.signal
import cc.protobuf.status
import cc.protobuf.rr

### Third-party modules
import google.protobuf.message

### Standard Python modules
import logging
import argparse
import sys
import os.path

### Import well-known ProtoBuf modules from Google. Their symbols will appear within
### the namespace that matches their `package` declarations: `google.protobuf`.
import_wellknown_protos(globals())

### Import core ProtoBuf modules. Their symbols will appear within namespaces
### that matches their respective `package` declarations (starting with `cc.`).
import_core_protos(globals())

### Import ProtoBuf data types from `demo.proto`.  These will appear in the
### namespace matching its package declaration: `cc.demo'.
import_proto('demo', globals())

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

def legend():
    '''Interactive Service Control.  Subsystems loaded:

        demo_grpc - Simple gRPC communications example
        demo_zmq - Simple 0MQ communications example

    ProtoBuf types are generally loaded into namespaces matching the package
    names from their respective `.proto` files:

        google.protobuf - Well-known types from Google
        cc.* - Various custom types
        cc.protobuf.* - General utilities and wrapper modules

    Use 'help(subsystem)' to list available subcomponents or methods
    '''
    print(legend.__doc__)


def main():
    args   = ArgParser().parse_args()

    logger = logging.getLogger()
    logger.setLevel((logging.INFO, logging.DEBUG)[args.debug])

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

if __name__ == '__main__':
    main()
