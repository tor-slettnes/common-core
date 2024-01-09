#!/usr/bin/python3 -i
#===============================================================================
## @file demoshell.py
## @brief Interactive service control via collection of clients.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Make the contents of Python client modules available in namespaces roughly
### corresponding to the package names of the corresponding `.proto` files

import grpc_client
import demo_client

from protobuf_standard_types import ProtoBuf

### Container class for ProtoBuf message types from mutiple services
### (e.g. "CC.Demo.Signal").

class CC (demo_client.CC):
    pass

### Add a few arguments to the base argparser
class ArgParser (grpc_client.ArgParser):
    def __init__ (self, host=None, identity="DemoShell", *args, **kwargs):
        super().__init__(host, *args, **kwargs);

        self.add_argument('--identity',
                          type=str,
                          default=identity,
                          help="Identity of this client")

def legend ():
    '''
    Interactive Service Control.  Subsystems loaded:

        demo   - DemoClient: Simple Reference Application

    Use 'help(subsystem)' to list available methods
    '''
    print(legend.__doc__)

if __name__ == "__main__":
    args      = ArgParser().parse_args()

    demo      = demo_client.DemoClient(args.host)
    legend()
