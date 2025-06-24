'''
Generic communications endpoint
'''

__all__ = ['Endpoint']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


import sys, re, socket, logging, argparse

from ...core.settingsstore import SettingsStore
from ...buildinfo import PROJECT_NAME

#===============================================================================
# Base class

class Endpoint (object):
    # `messaging_flavor` should be overwritten by direct subclasses to indicate
    # message platform, e.g., `gRPC`, `DDS`, `ZMQ`, ...
    messaging_flavor = None

    # `endpoint_type` should be overwritten by intermediate subclasses to
    # indicate role (`"client"`, `"service"`, `"publisher"`, `"subscriber"`, ...)
    endpoint_type = None

    # `channel_name` should be overwritten by final subclass to look up settings
    # for this communications channel (e.g., protocol/host/port, ...)
    channel_name = None

    # If `project_name` is provided as a class attribute or as an argument to
    # `__init__()`, it will be used to look up service settings from
    # project-specific settings files, e.g.
    #`*messaging_flavor*-endpoints-*project_name*.yaml`
    project_name = PROJECT_NAME


    def __init__(self,
                 channel_name: str|None = None,
                 project_name: str|None = None):

        if channel_name is not None:
            self.channel_name = channel_name
        else:
            assert self.channel_name is not None, \
                "Messaging Endpoint subclass %s.%s should set 'channel_name' -- see %s"%\
                (type(self).__module__, type(self).__name__, __file__)

        if project_name is None:
            project_name = type(self).project_name

        settings_files = [self.settings_file(flavor)
                          for flavor in (channel_name, project_name, "common")
                          if flavor]

        self.settings = SettingsStore(settings_files)
        logging.debug("Loaded settings from %s with search path %s: %s"%
                      (settings_files, self.settings.searchpath, self.settings))


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
