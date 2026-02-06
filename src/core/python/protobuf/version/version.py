'''
version.py - Utility functions for data types from `version.proto`
'''

__author__ = "Tor Slettnes"
__docformat__ = "javadoc en"


### Standard Python modules
import re

### Modules within package
from .version_pb2 import Version, ComponentVersions

_rx_version = re.compile(r"(\d+)(?:\.(\d+))?(?:\.(\d+))?(?:-(\d+))?.*")

def create_version(major: int,
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

    if version.printable_version:
        return version.printable_version
    else:
        return "%d.%d.%d"%(version.major, version.minor, version.patch)


def version_from_string(string: str) -> Version:
    '''
    Parse version string
    '''

    version = Version(printable_version = string)
    if m := _rx_version.match(string):
        version.major, version.minor, version.patch, version.build_number = \
            [int(group or 0) for group in m.groups()]
    return version


def is_compatible_version(client_version : Version,
                          server_version : Version,
                          strict: bool = True) -> bool:

    '''
    Indicate whether the provided client and server versions expected to be
    compatible
    '''

    return ((client_version.major == server_version.major) and
            ((client_version.minor <= server_version.minor) or not strict))


