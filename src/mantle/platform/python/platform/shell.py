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
from .upgrade.grpc.client import UpgradeClient
from cc.multilogger.grpc.client import LogClient

import cc.protobuf.wellknown
import cc.protobuf.variant
import cc.protobuf.signal
import cc.protobuf.status
import cc.protobuf.sysconfig
import cc.protobuf.vfs
import cc.protobuf.upgrade

### Third-party modules
import google.protobuf.message

### Standard Python modules
import logging
import argparse
import sys
import os.path

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

        self.add_argument('--identity',
                          type=str,
                          default=identity,
                          help='Client identification')

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
    '''
    Interactive Service Control.  Subsystems loaded:

        logger    - `Logger` gRPC service client
        sysconfig - `SysConfig` gRPC service client
        netconfig - `NetConfig` gRPC service client
        vfs       - `VirtualFileSystem` gRPC service client
        upgrade   - `Upgrade` gRPC service client

    Generated ProtoBuf data types and associated wrapper methods are generally
    available in the `cc.protobuf` namespace, e.g.:

      - cc.protobuf.sysconfig - Data types for the SysConfig service
      - cc.protobuf.netconfig - Data types for the NetConfig service
      - cc.protobuf.vfs       - Data types for the VirtualFileSystem service
      - cc.protobuf.upgrade   - Data types for the Upgrade service
      - cc.protobuf.wellknown - Well-known types from Google

    Use 'help(subsystem)' to list available subcomponents or methods
    '''
    print(legend.__doc__)


if __name__ == "__main__":
    args   = ArgParser().parse_args()

    logging.getLogger().setLevel(logging.DEBUG if args.debug else logging.INFO)

    logger = LogClient(
        args.host,
        wait_for_ready = args.wait_for_ready,
        capture_python_logs = True)

    sysconfig = SysConfigClient(
        args.host,
        wait_for_ready = args.wait_for_ready)

    netconfig = NetConfigClient(
        args.host,
        wait_for_ready = args.wait_for_ready)

    vfs = VirtualFileSystemClient(
        args.host,
        wait_for_ready = args.wait_for_ready)

    upgrade = UpgradeClient(
        args.host,
        wait_for_ready = args.wait_for_ready)

    logger.initialize()
    sysconfig.initialize()
    netconfig.initialize()
    vfs.initialize()
    upgrade.initialize()

    legend()
