#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file version.py
## @brief Utility functions for data types from `version.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.protobuf.core.version_pb2 import Version, ComponentVersions

def version(major: int,
            minor: int = 0,
            patch: int = 0,
            printable_version: str|None = None) -> Version:
    '''
    Return a new ProtoBuf Version message with the specified values
    '''

    return Version(major=major,
                   minor=minor,
                   patch=patch,
                   printable_version=printable_version)


def version_to_string(version: Version) -> str:
    '''
    Return a string representation of the provided ProtoBuf Version message
    '''

    if version.printed_version:
        return version.printed_version
    else:
        return "%d.%d.%d"%(version.major, version.minor, version.patch)


def is_compatible(client_version : Version,
                  server_version : Version,
                  strict: bool = True) -> bool:

    '''
    Indicate whether the provided client and server versions expected to be
    compatible
    '''

    return ((client_version.major == server_version.major) and
            ((client_version.minor <= server_version.minor) or not strict))
