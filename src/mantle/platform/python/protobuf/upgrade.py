#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file upgrade.py
## @brief Utility functions for data types from `upgrade.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.protobuf.import_proto import import_proto
from cc.protobuf.wellknown import decodeTimestamp
from cc.protobuf.variant import decodeValueMap
from cc.protobuf.vfs import VFSPathType, VFSPathsType, encodePath, decodePath
from cc.core.enumeration import Enumeration

### Standard Python modules
from typing import Sequence, Mapping, Tuple
from collections import namedtuple
import re
import os.path

## Import generated ProtoBuf symbols. These will appear in the namespaces
## corresponding to the package names specified in their respective `.proto`
## files (e.g. `cc.platform.upgrade`).
import_proto('upgrade', globals())
import_proto('vfs', globals())


#===============================================================================
## Annotation types

SourceType = cc.platform.upgrade.PackageSource | VFSPathType | str


#===============================================================================
## Methods

def encodeSource(source: SourceType,
                 is_file: bool = False) -> cc.platform.upgrade.PackageSource:
    '''Encode an package source from either an existing PackageSource instance
    (no conversion done), a VFS Path, or an URL.'''


    if isinstance(source, cc.platform.upgrade.PackageSource):
        return source

    elif isinstance(source, cc.platform.vfs.Path):
        return cc.platform.upgrade.PackageSource(vfs_path = source)

    elif isinstance(source, str) and source:
        if re.match(r'\w+://', source):
            return cc.platform.upgrade.PackageSource(url = source)

        else:
            return cc.platform.upgrade.PackageSource(vfs_path = encodePath(source))

    elif source:
        raise TypeError('`source` must be of type `cc.platform.upgrade.Source`, '
                        '`cc.platform.vfs.Path` or a string representing an URL '
                        'or a VFS path in the format `CONTEXT:PATH`')

    else:
        return cc.platform.upgrade.PackageSource()


def decodeSource(source: cc.platform.upgrade.PackageSource) -> str:
    '''Decode a package source to either a URL or a VFS path in the format `CONTEXT:PATH`'''

    if source.HasField('vfs_path'):
        path = decodePath(source.vfs_path)

    elif source.HasField('url'):
        path = source

    else:
        path = ''

    if source.filename:
        path = os.path.join(path, filename)

    return path
