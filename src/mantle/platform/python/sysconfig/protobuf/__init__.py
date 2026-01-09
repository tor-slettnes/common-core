#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file sysconfig.py
## @brief Utility functions for data types from `sysconfig.proto`
## @author Tor Slettnes
#===============================================================================

### Generated from `.../protos/cc/platform/sysconfig/protobuf/sysconfig_types.proto`
from .sysconfig_types_pb2 import *
from .sysconfig_types_pb2 import TimeZoneCountry

from cc.protobuf.utils import native_enum_from_proto

TimeSync = native_enum_from_proto(TimeSync)

def encodeCountry(country: str|None, required=False):
    result = TimeZoneCountry()

    if isinstance(country, str):
        if (len(country) == 2) and country.isupper():
            result.code = country
        else:
            result.name = country

    elif isinstance(country, TimeZoneCountry):
        result = country

    elif not country and not required:
        pass

    else:
        raise TypeError("Expected country code or name")

    return result
