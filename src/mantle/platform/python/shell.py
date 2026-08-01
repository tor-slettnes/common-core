#!/usr/bin/python3 -i
#===============================================================================
## @file shell.py
## @brief Interactive service control via collection of clients.
## @author Tor Slettnes
#===============================================================================

### Modules within package
from .sysconfig.grpc import Client as SysConfigClient
from .netconfig.grpc import Client as NetConfigClient
from .vfs.grpc import Client as VirtualFileSystemClient
from .upgrade.grpc import Client as UpgradeClient
from .switchboard.grpc import Client as SwitchboardClient
from .multilogger.grpc import Client as MultiLoggerClient
from .pubsub.grpc import Client as PubSubClient

import cc.core
import cc.protobuf.datetime
import cc.protobuf.request_reply
import cc.protobuf.signal
import cc.protobuf.status
import cc.protobuf.variant
import cc.protobuf.version
import cc.protobuf.wellknown
import cc.protobuf.utils
import cc.protobuf.builder
import cc.protobuf.dissecter

import cc.platform.pubsub.protobuf
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

      pubsub       - `cc.platform.pubsub.grpc.Relay` client
      multilogger  - `cc.platform.multilogger.grpc.MultiLogger` client
      switchboard  - `cc.platform.switchboard.grpc.Switchboard` client
      sysconfig    - `cc.platform.sysconfig.grpc.SysConfig` client
      netconfig    - `cc.platform.netconfig.grpc.NetConfig` client
      vfs          - `cc.platform.vfs.grpc.VirtualFileSystem` client
      upgrade      - `cc.platform.upgrade.grpc.Upgrade` service client

    Generated ProtoBuf data types are available in various namespaces, e.g.:

      cc.protobuf.*          - Common ProtoBuf types
      cc.platform.*.protobuf - ProtoBuf types for each service above

    Finally:

      cc.core.*    - Miscellaneous common utility methods and types

    Use 'help(component)' to inspect each of the above components or namespaces
    '''
    print(legend.__doc__)


if __name__ == "__main__":
    args   = ArgParser().parse_args()

    cc.core.logbase.init_logging(logging.DEBUG if args.debug else logging.INFO)

    pubsub = relay = PubSubClient(
        args.host,
        wait_for_ready = args.wait_for_ready)

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

    pubsub.initialize()
    multilogger.initialize()
    switchboard.initialize()
    sysconfig.initialize()
    netconfig.initialize()
    vfs.initialize()
    upgrade.initialize()

    legend()
