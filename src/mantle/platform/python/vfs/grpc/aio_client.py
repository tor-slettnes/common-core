'''
Python AsyncIO client for `VirtualFileSystem` gRPC service
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard Python modules
from typing import Mapping, Sequence, Iterator
import pathlib

### Common Core modules
from cc.core.decorators import override
from cc.protobuf.variant import PyValue, decodeKeyValueMap
from cc.messaging.grpc.client import AsyncMixIn

### VFS modules
from ..protobuf import (
    ContextSpec,
    VFSPathInput, VFSPathInputs,
    FileStats, FileInfo, FileChunk, VolumeInfo,
    encodePath, decodeFileInfo,
    pathRequest, locateRequest, attributeRequest,
)
from .base_client import BaseClient, LocalFile
from .signal_mixin import SignalMixIn

#===============================================================================
## SimpleAsyncClient

class SimpleAsyncClient (AsyncMixIn, BaseClient):
    '''
    Python AsyncIO client for VirtualFileSystem gRPC service.

    For a flavor that passively listens for, caches, and forwards update events
    (signals) from the server, see `AsyncClient`.
    '''

    @override
    async def get_contexts(self,
                           open_only: bool = False,
                           removable_only: bool = False,
                           ) -> Mapping[str, ContextSpec]:

        contextmap = await BaseClient.get_contexts(**locals())
        return contextmap.map

    @override
    async def get_context(self, name: str) -> ContextSpec:
        return await BaseClient.get_context(**locals())

    @override
    async def open_context(self, name: str) -> ContextSpec:
        await BaseClient.open_context(**locals())

    @override
    async def close_context(self, name: str):
        await BaseClient.close_context(**locals())

    @override
    async def get_volume_info(self,
                              vfspath: VFSPathInput,
                              dereference: bool =False) -> VolumeInfo:
        return await BaseClient.get_volume_info(**locals())

    @override
    async def get_file_info(self,
                            vfspath: VFSPathInput,
                            dereference: bool = True,
                            with_attributes: bool = True,
                            ) -> FileStats:

        fileinfo = await BaseClient.get_file_info(**locals())
        return decodeFileInfo(fileinfo)


    @override
    async def get_directory(self,
                      vfspath: VFSPathInput,
                      dereference: bool = True,
                      with_attributes: bool = True,
                      ) -> Mapping[str, FileStats]:

        directory = await BaseClient.get_directory(**locals())
        return {
            key: decodeFileInfo(info)
            for (key, info) in directory.map.items()
        }


    @override
    async def list(self,
             vfspath: VFSPathInput,
             dereference: bool = True,
             ) -> Sequence[str]:

        directory = await BaseClient.list(**locals())
        return list(directory.map)


    @override
    async def locate(self,
               vfspath: VFSPathInput,
               filename_masks: VFSPathInputs,
               attribute_filters: dict = {},
               with_attributes: bool = True,
               include_hidden: bool = False,
               ignore_case: bool = False,
               ) -> Mapping[str, FileStats]:

        entries = await BaseClient.locate(**locals())
        return {
            key: decodeFileInfo(info)
            for (key, info) in entries.map.items()
        }


    @override
    async def copy(self,
                   sources: str|Sequence[str],
                   target: str,
                   force: bool = False,
                   dereference: bool = False,
                   merge: bool = False,
                   update: bool = False,
                   with_attributes: bool = True,
                   inside_target: bool = False):
        await BaseClient.copy(**locals())

    @override
    async def move(self,
                   sources: str|Sequence[str],
                   target: str,
                   force: bool = False,
                   dereference: bool = False,
                   merge: bool = False,
                   update: bool = False,
                   with_attributes: bool = True):
        await BaseClient.move(**locals())

    @override
    async def create_folder(self,
                            vfspath: VFSPathInput,
                            force: bool = False,
                            dereference: bool = False):
        await BaseClient.create_folder(**locals())

    @override
    async def remove(self,
                     paths: VFSPathInputs,
                     force: bool = False,
                     dereference: bool = False,
                     with_attributes: bool = True):
        await BaseClient.remove(**locals())

    @override
    async def read(self, vfspath: VFSPathInput) -> Iterator[bytes]:
        async for chunk in self.read_file(vfspath):
            yield chunk.data

    @override
    async def read_file(self, vfspath: VFSPathInput) -> Iterator[FileChunk]:
        return await self.stub.ReadFile(encodePath(vfspath))

    @override
    async def download(self,
                       vfspath: VFSPathInput,
                       localfile: LocalFile):

        if isinstance(localfile, pathlib.Path):
            localfile = localfile.open("wb")

        elif isinstance(localfile, str):
            localfile = open(localfile, "wb")

        async for chunk in self.read_file(vfspath):
            localfile.write(chunk.data)

    @override
    async def upload(self,
                     localfile: LocalFile,
                     vfspath: VFSPathInput):
        await BaseClient.upload(**locals())

    @override
    async def get_attributes(self, vfspath: VFSPathInput) -> Mapping[str, PyValue]:
        kvmap = await BaseClient.get_attributes(**locals())
        return decodeKeyValueMap(kvmap)

    @override
    async def set_attributes(self, vfspath: VFSPathInput, **attributes):
        await BaseClient.set_attributes(**locals())

    @override
    async def clear_attributes(self, vfspath: VFSPathInput):
        await BaseClient.clear_attributes(**locals())



#===============================================================================
## AsyncClient

class AsyncClient (SignalMixIn, SimpleAsyncClient):
    '''
    AsyncIO client for the gRPC VirtualFileSystem service with signal
    listener.

    This specializes `SimpleAsyncClient` by passively listening for update
    events (signals) from the server.  To subscribe to one or more of these
    signals, connect your own callback handler using one of the provided
    `connect_*()` methods.  Additionally, property methods are provided to
    obtain recevied information from the local signal cache.
    '''
