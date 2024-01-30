#!/usr/bin/python3 -i
#===============================================================================
## @file demoshell.py
## @brief Interactive service control via collection of clients.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Make the contents of Python client modules available in namespaces roughly
### corresponding to the package names of the corresponding `.proto` files

from ipc.google.protobuf     import ProtoBuf
from ipc.google.grpc.client  import ArgParser as _ArgParser

import demo_grpc_client
import demo_zmq_client
import logging

### Container class for ProtoBuf message types from mutiple services
### (e.g. "CC.Demo.Signal").

class CC (demo_grpc_client.CC):
    pass

### Add a few arguments to the base argparser
class ArgParser (_ArgParser):
    def __init__ (self, host=None, identity="DemoShell", *args, **kwargs):
        super().__init__(host, *args, **kwargs);

        self.add_argument('--identity',
                          type=str,
                          default=identity,
                          help="Identity of this client")

        self.add_argument('--debug',
                          default=False,
                          action='store_const',
                          const=True,
                          help='Print debug messages')


def legend ():
    '''
    Interactive Service Control.  Subsystems loaded:

        demo_grpc  - Simple gRPC communications example
        demo_zmq   - Simple ZeroMQ communications example

    Use 'help(subsystem)' to list available methods
    '''
    print(legend.__doc__)

if __name__ == "__main__":
    args   = ArgParser().parse_args()

    logger = logging.getLogger()
    logger.setLevel((logging.INFO, logging.DEBUG)[args.debug])

    demo_grpc = demo_grpc_client.DemoClient(args.host)
    demo_zmq  = demo_zmq_client.DemoClient(args.host)
    legend()
