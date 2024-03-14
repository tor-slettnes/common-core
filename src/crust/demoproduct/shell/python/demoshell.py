#!/usr/bin/python3 -i
#===============================================================================
## @file demoshell.py
## @brief Interactive service control via collection of clients.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
import demo.core.types
import demo.grpc.client
import demo.zmq.client

### Standard Python modules
import logging
import argparse
import sys
import os.path
import asyncio

### Make the contents of Python client modules available in namespaces roughly
### corresponding to the package names of the corresponding `.proto` files
from cc.io.protobuf import ProtoBuf

### Container class for ProtoBuf message types from mutiple services
### (e.g. "CC.Demo.Signal").

class CC (demo.core.types.CC):
    pass

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

        self.add_argument('--asyncio',
                          default=False,
                          action='store_const',
                          const=True,
                          help="Create client with AsyncIO semantics")

        self.add_argument('--debug',
                          default=False,
                          action='store_const',
                          const=True,
                          help='Print debug messages')


def legend():
    '''
    Interactive Service Control.  Subsystems loaded:

        demo_grpc  - Simple gRPC communications example
        demo_zmq   - Simple 0MQ communications example

    Use 'help(subsystem)' to list available methods
    '''
    print(legend.__doc__)

def asyncio_legend():
    '''
    NOTE: Using Python AsyncIO semantics. To invoke a unary gRPC method, use:

          main_loop.run_until_complete(demo_grpc.METHOD(...))

    For streaming methods, use:

          demo_grpc.start_notify_time(callback)
          demo_grpc.start_notify_greetings(callback)
          main_loop.run_forever()
    '''
    print(asyncio_legend.__doc__)


if __name__ == "__main__":
    args   = ArgParser().parse_args()

    logger = logging.getLogger()
    logger.setLevel((logging.INFO, logging.DEBUG)[args.debug])

    demo_grpc = demo.grpc.client.DemoClient(args.host,
                                            identity = args.identity,
                                            use_asyncio = args.asyncio)

    demo_zmq  = demo.zmq.client.DemoClient(args.host,
                                           identity = args.identity)

    demo_grpc.initialize()
    demo_zmq.initialize()
    legend()

    if args.asyncio:
        main_loop = asyncio.get_event_loop()
        asyncio_legend()
