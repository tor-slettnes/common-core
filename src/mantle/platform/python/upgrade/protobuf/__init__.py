#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file upgrade.py
## @brief Utility functions for data types from `upgrade.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Standard Python modules
from typing import Sequence, Mapping, Tuple
from collections import namedtuple
import re
import os.path

### Modules within package
from cc.protobuf.utils import proto_enum
from ...vfs.protobuf import Path as VFSPath, VFSPathType, encodePath, decodePath
from .upgrade_types_pb2 import *
from .upgrade_types_pb2 import PackageSource

#===============================================================================
## Annotation types

UpgradeState = proto_enum(UpgradeState)
SourceType = PackageSource | VFSPathType | str

#===============================================================================
## Methods

def encodeSource(source: SourceType) -> PackageSource:
    '''
    Encode an package source from either an existing PackageSource instance
    (no conversion done), a VFS Path, or an URL.
    '''


    if isinstance(source, PackageSource):
        return source

    elif isinstance(source, VFSPath):
        return PackageSource(vfs_path = source)

    elif isinstance(source, str) and source:
        if re.match(r'\w+://', source):
            return PackageSource(url = source)

        else:
            return PackageSource(vfs_path = encodePath(source))

    elif source:
        raise TypeError('`source` must be of type `cc.protobuf.upgrade.Source`, '
                        '`cc.protobuf.vfs.Path` or a string representing an URL '
                        'or a VFS path in the format `CONTEXT:PATH`')

    else:
        return PackageSource()


def decodeSource(source: PackageSource) -> str:
    '''
    Decode a package source to either a URL or a VFS path in the format
    `CONTEXT:PATH`
    '''

    if source.HasField('vfs_path'):
        path = decodePath(source.vfs_path)

    elif source.HasField('url'):
        path = source

    else:
        path = ''

    if source.filename:
        path = os.path.join(path, filename)

    return path
