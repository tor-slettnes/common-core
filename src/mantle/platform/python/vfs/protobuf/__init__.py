#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file vfs.py
## @brief Utility functions for data types from `vfs.proto`
## @author Tor Slettnes
#===============================================================================

### Generated from `.../protos/cc/platform/vfs/protobuf/vfs_types.proto`
from .vfs_types_pb2 import *
from .vfs_types_pb2 import \
    PathType, Path, PathRequest, LocateRequest, AttributeRequest, FileInfo

### Modules within package
from cc.protobuf.utils import native_enum_from_proto
from cc.protobuf.wellknown import decodeTimestamp
from cc.protobuf.variant import PyValueDict, PyTaggedValueList, decodeKeyValueMap
from cc.core.types import OCT8

### Standard Python modules
from typing import Sequence, Mapping, Tuple
from collections import namedtuple


PathType = native_enum_from_proto(PathType)

FileStatus = namedtuple("FileStatus",
                        ("type", "size", "link",
                         "mode", "readable", "writable",
                         "uid", "gid", "ownername", "groupname",
                         "access_time", "modify_time", "create_time",
                         "attributes"))

#===============================================================================
## Annotation types

VFSPathType = str | Path
VFSPathsType = VFSPathType | Sequence[VFSPathType]

#===============================================================================
## Methods

def encodePath(vfspath: VFSPathType) -> Path:
    '''
    Encode a VFS path

    @param vfspath:
        Either an existing `cc.protobuf.vfs.Path` instance (no conversion done)
        or a string in the format `CONTEXT:RELPATH`.
    '''

    if isinstance(vfspath, Path):
        return vfspath

    elif isinstance(vfspath, str):
        try:
            context, relpath = vfspath.split(":", 1)
        except ValueError:
            context, relpath = vfspath, ""

        return Path(context=context, relpath=relpath.lstrip("/"))

    else:
        raise TypeError("VFS path must be of type 'str' or 'cc.protobuf.vfs.Path'", vfspath)

def encodePaths(vfspaths: VFSPathsType) -> list[Path]:
    '''
    Encode a sequence of VFS paths.
    '''

    if isinstance(vfspaths, str):
        vfspaths = (vfspaths,)

    return [encodePath(vpath) for vpath in vfspaths]

def decodePath(vfspath: Path) -> str:
    '''
    Encode a VFS path into a string in the format `CONTEXT:PATH`.
    '''

    return ":".join((vfspath.context, vfspath.relpath))

def decodeStats(stats: FileInfo) -> FileStatus:
    return FileStatus(
        PathType.get(stats.type, PathType.TYPE_NONE),
        stats.size, stats.link,
        OCT8(stats.mode), stats.readable, stats.writable,
        stats.uid, stats.gid, stats.ownername, stats.groupname,
        decodeTimestamp(stats.access_time),
        decodeTimestamp(stats.modify_time),
        decodeTimestamp(stats.create_time),
        decodeKeyValueMap(stats.attributes))

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

    return PathRequest(**kwargs)

def locateRequest(root: VFSPathType,
                  filename_masks: str|Sequence[str],
                  attribute_filters: PyValueDict | PyTaggedValueList,
                  with_attributes: bool,
                  include_hidden: bool,
                  ignore_case: bool):

    if isinstance(filename_masks, str):
        filename_masks = [filename_masks]

    if isinstance(attribute_filters, dict):
        attribute_filters = attribute_filters.items();

    return LocateRequest(
        root             = encodePath(root),
        filename_masks   = filename_masks,
        attribute_filter = [ encodeValue(tv) for tv in attribute_filters ],
        with_attributes  = with_attributes,
        include_hidden   = include_hidden,
        ignore_case      = ignore_case)

def attributeRequest(vfspath: VFSPathType,
                     attributes: Mapping[str, object]):

    if isinstance(attributes, dict):
        attributes = attributes.items()

    path   = encodePath(vfspath)
    tvlist = [ encodeValue(v) for v in attributes ]
    return AttributeRequest(path=path, attributes=tvlist)
