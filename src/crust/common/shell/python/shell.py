#!/usr/bin/python3 -i
#===============================================================================
## @file shell.py
## @brief Interactive service control, shared components loaded
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from cc.messaging.grpc.client import ArgParser as _ArgParser
import cc.protobuf.wellknown
import cc.protobuf.variant
import cc.protobuf.signal
import cc.protobuf.status
import cc.protobuf.rr

### Third-party modules
import google.protobuf.message

### Standard Python modules
import logging

### Import well-known ProtoBuf modules from Google.
### Their symbols will appear within the `google.protobuf` namespace.
from cc.protobuf.wellknown import import_wellknown_protos
import_wellknown_protos(globals())

### Add a few arguments to the base argparser
class ArgParser (_ArgParser):
    def __init__ (self, host=None, identity="DemoShell", *args, **kwargs):
        super().__init__(host, *args, **kwargs);

        self.add_argument('--debug',
                          default=False,
                          action='store_const',
                          const=True,
                          help='Print debug messages')


def legend ():
    '''
    Interactive Python shell with ProtoBuf data types preloaded
    into namespaces corresponding to their respective package names:

        google.protobuf.*   - well-known types
        cc.protobuf.*       - custom types

    '''
    print(legend.__doc__)

if __name__ == "__main__":
    args   = ArgParser().parse_args()

    logger = logging.getLogger()
    logger.setLevel((logging.INFO, logging.DEBUG)[args.debug])

    legend()
