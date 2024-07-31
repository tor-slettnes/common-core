#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file sysconfig.py
## @brief Utility functions for data types from `sysconfig.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.protobuf.import_proto import import_proto

## Import generated ProtoBuf symbols from `vfs.proto`. These will appear in
## the namespace corresponding to its package name, `cc.platform.vfs`.
import_proto('sysconfig', globals())


def encodeCountry(country: str, allow_empty=False):
    result = cc.platform.sysconfig.TimeZoneCountry()

    if isinstance(country, str):
        if (len(country) == 2) and country.isupper():
            result.code = country
        else:
            result.name = country

    elif isinstance(country, cc.platform.sysconfig.TimeZoneCountry):
        result = country

    elif not country and allow_empty:
        pass

    else:
        raise TypeError("Expected country code or name")

    return result
