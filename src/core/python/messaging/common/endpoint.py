'''
Generic communications endpoint
'''

__all__ = ['Endpoint']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


import sys, re, socket, argparse

from ...core.settingsstore import SettingsStore
from ...core.logbase import LogBase

#===============================================================================
# Base class

class Endpoint (LogBase):
    # `messaging_flavor` should be overwritten by direct subclasses to indicate
    # message platform, e.g., `gRPC`, `DDS`, `ZMQ`, ...
    messaging_flavor = None

    # `endpoint_type` should be overwritten by intermediate subclasses to
    # indicate role (`"client"`, `"service"`, `"publisher"`, `"subscriber"`, ...)
    endpoint_type = None

    # `channel_name` should be overwritten by final subclass to look up settings
    # for this communications channel (e.g., protocol/host/port, ...)
    channel_name = None

    # If `product_name` is provided as a class attribute or as an argument to
    # `__init__()`, it will be used to look up service settings from
    # product-specific settings files, e.g.
    #`*messaging_flavor*-endpoints-*product_name*.yaml`
    product_name = None

    # `project_name` may be overwritten by subclasses to look up settings for a
    # particular code project (e.g., a parent code repository).
    project_name = None


    def __init__(self,
                 channel_name: str|None = None,
                 project_name: str|None = None,
                 product_name: str|None = None):

        '''
        Initializer.  Parameters:

        @param channel_name
            Messaging channel name, e.g. gRPC service name. Use to look up
            endpoint settings.

        @param project_name
            Name of code project (e.g. parent code repository). Used to look up
            endpoint settings.

        @param product_name
            Name of overall product. Used to look up endpoint settings.
            (Since this is Python, it must be provided as a runtime argument
            rather than a pre-built default).
        '''


        if channel_name is not None:
            self.channel_name = channel_name
        else:
            assert self.channel_name is not None, \
                "Messaging Endpoint subclass %s.%s should set 'channel_name' -- see %s"%\
                (type(self).__module__, type(self).__name__, __file__)

        if project_name is not None:
            self.project_name = project_name

        if product_name is not None:
            self.product_name = product_name

        settings_scopes = (
            self.channel_name,
            self.product_name,
            self.project_name,
            "common")

        self.settings = SettingsStore([
            self.settings_file(scope_name)
            for scope_name in settings_scopes
            if scope_name])

        self.logger.debug("Loaded %s settings from %s: %s"%
                          (self.channel_name, self.settings.filepaths, self.settings))


    def __repr__ (self):
        return "%s %s %s"%(self.messaging_flavor, self.channel_name, self.endpoint_type)

    def initialize(self):
        pass

    def deinitialize(self):
        pass

    def settings_file(self, scope_name):
        return "%s-endpoints-%s"%(self.messaging_flavor.lower(), scope_name.lower())

    def setting(self,
                key     : str,
                default : str = None) -> object:
        try:
            return self.settings[self.channel_name][key]
        except (TypeError, KeyError):
            return self.settings.get("_default_", {}).get(key, default)
