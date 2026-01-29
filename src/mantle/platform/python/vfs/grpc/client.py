'''
Python client for `VirtualFileSystem` gRPC service
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'


### Standard Python modules
from typing import Mapping, Sequence, Iterator
import pathlib

### Common Core modules
from cc.core.decorators import override
from cc.protobuf.variant import PyValue, decodeKeyValueMap

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
## SimpleClient

class SimpleClient (BaseClient):
    '''
    Stateless client for the VirtualFileSystem gRPC service.

    For a flavor that passively listens for, caches, and forwards update events
    (signals) from the server, see `Client`.
    '''

    @override
    def get_contexts(self,
                     open_only: bool = False,
                     removable_only: bool = False,
                     ) -> Mapping[str, ContextSpec]:

        contextmap = BaseClient.get_contexts(**locals())
        return contextmap.map

    @override
    def get_file_info(self,
                      vfspath: VFSPathInput,
                      dereference: bool = True,
                      with_attributes: bool = True,
                      ) -> FileStats:

        fileinfo = BaseClient.get_file_info(**locals())
        return decodeFileInfo(fileinfo)


    @override
    def get_directory(self,
                      vfspath: VFSPathInput,
                      dereference: bool = True,
                      with_attributes: bool = True,
                      ) -> Mapping[str, FileStats]:

        directory = BaseClient.get_directory(**locals())
        return {
            key: decodeFileInfo(info)
            for (key, info) in directory.map.items()
        }


    @override
    def list(self,
             vfspath: VFSPathInput,
             dereference: bool = True,
             ) -> Sequence[str]:

        directory = BaseClient.list(**locals())
        return list(directory.map)


    @override
    def locate(self,
               vfspath: VFSPathInput,
               filename_masks: VFSPathInputs,
               attribute_filters: dict = {},
               with_attributes: bool = True,
               include_hidden: bool = False,
               ignore_case: bool = False,
               ) -> Mapping[str, FileStats]:

        entries = BaseClient.locate(**locals())
        return {
            key: decodeFileInfo(info)
            for (key, info) in entries.map.items()
        }


    @override
    def read(self, vfspath: VFSPathInput) -> Iterator[bytes]:
        for chunk in self.read_file(vfspath):
            yield chunk.data


    @override
    def get_attributes(self, vfspath: VFSPathInput) -> Mapping[str, PyValue]:
        kvmap = BaseClient.get_attributes(**locals())
        return decodeKeyValueMap(kvmap)


#===============================================================================
## Client

class Client (SignalMixIn, SimpleClient):
    '''
    Client for the VirtualFileSystem gRPC service with signal listener.

    This specializes `SimpleClient` by passively listening for update events
    (signals) from the server.  To subscribe to one or more of these signals,
    connect your own callback handler using one of the provided `connect_*()`
    methods.  Additionally, property methods are provided to obtain recevied
    information from the local signal cache.
    '''
