#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file zmq_base.py
## @brief Wrapper for ZeroMQ-based applications
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ..common.channel import Channel
from buildinfo        import PROJECT_NAME

#===============================================================================
# Base class

class Base (ipc.channel.Channel):

    @property
    def settings(self):
        if Base.settings is None:
            Base.settings = SettingsStore([
                "zmq-channels-common.json",
                f"zmq-channels-{self.product_name}.json",
            ])
        return Base.settings


    def _joinAddress(self,
                     protocol : str,
                     name     : str,
                     port     : int):

        address = f"{protocol}://{name}"
        if port:
            address += f":{port}"
        return address


class ArgParser(argparse.ArgumentParser):
    def __init__ (self, *args, **kwargs):
        argparse.ArgumentParser.__init__(self, prog=sys.argv[0], *args, **kwargs)
