#!/usr/bin/python3 -i
#===============================================================================
## @file shell.py
## @brief Interactive service control, shared components loaded
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from ..protobuf.import_proto import import_wellknown_protos, import_core_protos

### Third-party modules
import google.protobuf.message

### Standard Python modules
import logging
import argparse

### Import well-known ProtoBuf modules from Google. Their symbols will appear within
### the namespace that matches their `package` declarations: `google.protobuf`.
import_wellknown_protos(globals())

### Import core protobuf modules. Their symbols will appear within namespaces
### that matches their respective `package` declarations (starting with `cc.`).
import_core_protos(globals())


### Add a few arguments to the base argparser
class ArgParser (argparse.ArgumentParser):
    def __init__ (self, host=None, identity="DemoShell", *args, **kwargs):
        super().__init__(host, *args, **kwargs);

        self.add_argument('--debug',
                          default=False,
                          action='store_const',
                          const=True,
                          help='Print debug messages')


def legend():
    '''
    Interactive Python shell with ProtoBuf data types preloaded
    into namespaces corresponding to their respective package names:

     - Well-known ProtoBuf types : google.protobuf.*
     - Custom ProtoBuf types     : cc.*
     - Wrapper modules           : protobuf.*

    '''
    print(legend.__doc__)

if __name__ == "__main__":
    args   = ArgParser().parse_args()

    logger = logging.getLogger()
    logger.setLevel((logging.INFO, logging.DEBUG)[args.debug])

    legend()
