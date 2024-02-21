#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file endpoint.py
## @brief Generic communications endpoint
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import sys, re, socket, logging, argparse

from buildinfo          import PROJECT_NAME
from core.settingsstore import SettingsStore

#===============================================================================
# Base class

class Endpoint (object):
    # `product_name` is used to lookup product-specific information,
    # and might be overridden in subclasses.
    product_name = PROJECT_NAME

    # `messaging_flavor` should be overwritten by direct subclasses to indicate
    # message platform, e.g., `gRPC`, `DDS`, `ZMQ`, ...
    messaging_flavor = None

    # `endpoint_type` should be overwritten by intermediate subclasses to
    # indicate role (`"client"`, `"service"`, `"publisher"`, `"subscriber"`, ...)
    endpoint_type = None

    # `channel_name` should be overwritten by final subclass to look up settings
    # for this communications channel (e.g., protocol/host/port, ...)
    channel_name = None

    def __init__(self, channel_name: str = None):
        if channel_name is not None:
            self.channel_name = channel_name
        else:
            assert self.channel_name is not None, \
                "Subclass %r should set 'channel_name' -- see %s"%\
                (type(self).__name__, __file__)

        settings_files = [
            self.settings_file(self.product_name),
            self.settings_file("common")
        ]
        self.settings = SettingsStore(settings_files)
        logging.debug("Loaded settings from %s: %s"%(settings_files, self.settings))


    def __repr__ (self):
        return "%s %r %s"%(self.messaging_flavor, self.channel_name, self.endpoint_type)

    def initialize(self):
        pass

    def deinitialize(self):
        pass

    def settings_file(self, product_name):
        return "%s-endpoints-%s.json"%(self.messaging_flavor.lower(), product_name.lower())

    def setting(self,
                key     : str,
                default : str = None) -> object:
        try:
            return self.settings[self.channel_name][key]
        except (TypeError, KeyError):
            return default
