'''
VFS ProtoBuf type wrapper
'''

### Generated from `.../protos/cc/platform/vfs/protobuf/vfs_types.proto`
from .vfs_types_pb2 import *
from .vfs import (
    PathType, FileStats, VFSPathInput, VFSPathInputs,
    encodePath, decodePath, encodePaths, decodeFileInfo,
    pathRequest, locateRequest, attributeRequest,
)
