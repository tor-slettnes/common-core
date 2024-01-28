#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file grpc_base.py
## @brief Wrapper for gRPC-based services, common to client & server
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import sys, re, socket, logging, argparse

from settingsstore    import SettingsStore
from buildinfo        import PROJECT_NAME
from ..protobuf       import ProtoBuf, CC
from ..common.channel import Channel, ArgParser

#===============================================================================
# Base class

class Base (Channel):
    # `service_name` should be overwritten by final subclass to look up settings
    # for this endpoint (e.g., protocol/host/port, ...).
    service_name = None

    def __init__ (self,
                  service_name: str = None
                  channel_name: str = None):

        if service_name:
            self.service_name = service_name
        else:
            assert self.service_name, \
                "Subclass %r should set `service_name` -- see %s"%\
                (type(self).__name__, __file__)

        if not channel_name:
            channel_name = self.channel_name or self.service_name

        super().__init__(channel_name)


    @property
    def settings(self):
        if Base.settings is None:
            Base.settings = SettingsStore([
                "grpc-services-common.json",
                f"grpc-services-{self.product_name}.json"])

        return Base.settings


    def _joinAddress(self,
                     protocol : str,
                     name     : str,
                     port     : int):

        return "%s:%s"%(name, port)

