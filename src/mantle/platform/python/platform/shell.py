#!/usr/bin/python3 -i
#===============================================================================
## @file shell.py
## @brief Interactive service control via collection of clients.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .sysconfig.grpc.client import SysConfigClient
from .netconfig.grpc.client import NetConfigClient
from .vfs.grpc.client import VirtualFileSystemClient

from cc.protobuf.import_proto \
    import import_wellknown_protos, import_core_protos, import_proto

import cc.protobuf.wellknown
import cc.protobuf.variant
import cc.protobuf.signal
import cc.protobuf.status

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

### Import generated ProtoBuf data types.  These will appear in namespaces
### corresponding to the package names in their respective `.proto` files:
### `cc.platform.sysconfig`, `cc.platform.netconfig`, `cc.platform.vfs`.
import_proto('sysconfig', globals())
import_proto('netconfig', globals())
import_proto('vfs', globals())

### Add a few arguments to the base argparser
class ArgParser (argparse.ArgumentParser):
    def __init__ (self,
                  prog     = os.path.basename(sys.argv[0]),
                  identity = "Python Shell",
                  *args, **kwargs):

        super().__init__(prog=prog, *args, **kwargs);

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

        sysconfig - `SysConfig` gRPC service client
        netconfig - `NetConfig` gRPC service client
        vfs       - `VirtualFileSystem` gRPC service client

    ProtoBuf types are generally loaded into namespaces matching the package
    names from their respective `.proto` files:

        google.protobuf - Well-known types from Google
        cc.* - Various custom types
        cc.protobuf.* - General utilities and wrapper modules

    Use 'help(subsystem)' to list available subcomponents or methods
    '''
    print(legend.__doc__)


if __name__ == "__main__":
    args   = ArgParser().parse_args()

    logger = logging.getLogger()
    logger.setLevel((logging.INFO, logging.DEBUG)[args.debug])

    sysconfig = SysConfigClient(
        args.host,
        wait_for_ready = args.wait_for_ready)

    netconfig = NetConfigClient(
        args.host,
        wait_for_ready = args.wait_for_ready)

    vfs = VirtualFileSystemClient(
        args.host,
        wait_for_ready = args.wait_for_ready)

    sysconfig.initialize()
    netconfig.initialize()
    vfs.initialize()

    legend()
