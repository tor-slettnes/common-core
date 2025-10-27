#!/usr/bin/python3 -i
#===============================================================================
## @file shell.py
## @brief Interactive service control via collection of clients.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .sysconfig.grpc.client import SignalClient as SysConfigClient
from .netconfig.grpc.client import SignalClient as NetConfigClient
from .vfs.grpc.client import SignalClient as VirtualFileSystemClient
from .upgrade.grpc.client import SignalClient as UpgradeClient
from .multilogger.grpc.client import Client as MultiLoggerClient
from .switchboard.grpc.client import Client as SwitchboardClient

import cc.core
import cc.protobuf.datetime
import cc.protobuf.import_proto
import cc.protobuf.request_reply
import cc.protobuf.signal
import cc.protobuf.status
import cc.protobuf.utils
import cc.protobuf.variant
import cc.protobuf.version
import cc.protobuf.wellknown

import cc.platform.multilogger.protobuf
import cc.platform.switchboard.protobuf
import cc.platform.sysconfig.protobuf
import cc.platform.upgrade.protobuf
import cc.platform.vfs.protobuf


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

        multilogger - `MultiLogger` gRPC service client
        switchboard - `Switchboard` gRPC service client
        sysconfig   - `SysConfig` gRPC service client
        netconfig   - `NetConfig` gRPC service client
        vfs         - `VirtualFileSystem` gRPC service client
        upgrade     - `Upgrade` gRPC service client

    Generated ProtoBuf data types are generally available in various `protobuf`
    namespaces, e.g.:

      - cc.protobuf.*                - Common data types
      - cc.platform.SERVICE.protobuf - Data types for each SERVICE listed above

    Use 'help(component)' to inspect each of the above components or namespaces
    '''
    print(legend.__doc__)


if __name__ == "__main__":
    args   = ArgParser().parse_args()

    logging.getLogger().setLevel(logging.DEBUG if args.debug else logging.INFO)

    multilogger = MultiLoggerClient(
        args.host,
        wait_for_ready = args.wait_for_ready,
        capture_python_logs = True)

    switchboard = SwitchboardClient(
        args.host,
        wait_for_ready = args.wait_for_ready)

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

    multilogger.initialize()
    switchboard.initialize()
    sysconfig.initialize()
    netconfig.initialize()
    vfs.initialize()
    upgrade.initialize()

    legend()
