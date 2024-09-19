#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file vfs.py
## @brief Utility functions for data types from `vfs.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.protobuf.import_proto import import_proto
from cc.protobuf.wellknown import decodeTimestamp
from cc.protobuf.variant import decodeValueMap
from cc.core.enumeration import Enumeration
from cc.core.scalar_types import OCT8

### Standard Python modules
from typing import Sequence, Mapping, Tuple
from collections import namedtuple

## Import generated ProtoBuf symbols from `vfs.proto`. These will appear in
## the namespace corresponding to its package name, `cc.platform.vfs`.
import_proto('vfs', globals())

#===============================================================================
## Native types

PathType = Enumeration(cc.platform.vfs.PathType.items())

FileInfo = namedtuple(
    "FileInfo",
    ("type", "size", "link",
     "mode", "readable", "writable",
     "uid", "gid", "ownername", "groupname",
     "accessTime", "modifyTime", "createTime",
     "attributes"))


#===============================================================================
## Annotation types

VFSPathType = str | cc.platform.vfs.Path
VFSPathsType = VFSPathType | Sequence[VFSPathType]

#===============================================================================
## Methods

def encodePath(vfspath: VFSPathType) -> cc.platform.vfs.Path:
    '''Encode a VFS path from either an existing VFS path (no conversion done)
    or from a string with format `CONTEXT:PATH`.'''

    if isinstance(vfspath, cc.platform.vfs.Path):
        return vfspath

    elif isinstance(vfspath, str):
        try:
            context, relpath = vfspath.split(":", 1)
        except ValueError:
            raise ValueError("Specify VFS path in the format CONTEXT:PATH", vfspath)
        else:
            return cc.platform.vfs.Path(context=context, relpath=relpath.lstrip("/"))

    else:
        raise TypeError("VFS path must be of type 'str' or 'cc.platform.vfs.Path'", vfspath)


def encodePaths(vfspaths: VFSPathsType) -> list[cc.platform.vfs.Path]:
    '''Encode a sequence of VFS paths.  Returns a list of the encoded instances.'''

    if isinstance(vfspaths, str):
        vfspaths = (vfspaths,)

    return [encodePath(vpath) for vpath in vfspaths]

def decodePath(vfspath: cc.platform.vfs.Path) -> str:
    '''Encode a VFS path into a string in the format `CONTEXT:PATH`.'''

    return ":".join((vfspath.context, vfspath.relpath))


def decodeStats(stats: cc.platform.vfs.FileInfo) -> FileInfo:
    return FileInfo(
        PathType.get(stats.type, PathType.TYPE_NONE),
        stats.size, stats.link,
        OCT8(stats.mode), stats.readable, stats.writable,
        stats.uid, stats.gid, stats.ownername, stats.groupname,
        decodeTimestamp(stats.accessTime),
        decodeTimestamp(stats.modifyTime),
        decodeTimestamp(stats.createTime),
        decodeValueMap(stats.attributes))


def pathRequest(path: VFSPathType|None = None,
                sources: VFSPathsType|None = None,
                force: bool = False,
                dereference: bool = True,
                merge: bool = False,
                update: bool = False,
                with_attributes: bool = True,
                inside_target: bool = False):

    kwargs = dict(force=force, dereference=dereference, merge=merge,
                  update=update, with_attributes=with_attributes,
                  inside_target=inside_target)
    if sources:
        kwargs.update(sources=encodePaths(sources))

    if isinstance(sources, (tuple, list)):
        kwargs.update(inside_target=True)

    if path:
        kwargs.update(path=encodePath(path))

    return cc.platform.vfs.PathRequest(**kwargs)


def locateRequest(root: VFSPathType,
                  filename_masks: str|Sequence[str],
                  attribute_filters: Mapping[str, object] | Sequence[Tuple[str, object]],
                  with_attributes: bool,
                  include_hidden: bool):

    if isinstance(filename_masks, str):
        filename_masks = [filename_masks]

    if isinstance(attribute_filters, dict):
        attribute_filters = attribute_filters.items();

    return cc.platform.vfs.LocateRequest(
        root             = encodePath(root),
        filename_mask    = filename_masks,
        attribute_filter = [ encodeValue(tv) for tv in attribute_filters ],
        with_attributes  = with_attributes,
        include_hidden   = include_hidden)

def attributeRequest(vfspath: VFSPathType,
                     attributes: Mapping[str, object]):

    if isinstance(attributes, dict):
        attributes = attributes.items()

    path   = encodePath(vfspath)
    tvlist = [ encodeValue(v) for v in attributes ]
    return cc.platform.vfs.AttributeRequest(path=path, attributes=tvlist)
