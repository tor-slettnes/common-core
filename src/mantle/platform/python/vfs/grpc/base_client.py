'''
Python base client for `VirtualFileSystem` gRPC service. In most cases you
want to use one of the derived classes `Client` or `AsyncClient` instead of this
one.
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard Python modules
from typing import abstractmethod, Mapping, Sequence, Iterator
import io
import pathlib

### Common Core modules
from cc.protobuf.variant import KeyValueMap
from cc.messaging.grpc.client import GenericClient

### VFS modules
from ..protobuf import (
    GetContextsRequest, ContextSpec, ContextMap, Directory,
    Path as VFSPath, VFSPathInput, VFSPathInputs,
    FileInfo, FileChunk, VolumeInfo,
    encodePath, pathRequest, locateRequest, attributeRequest,
)

LocalPath = pathlib.Path | str
LocalFile = LocalPath | io.IOBase


#===============================================================================
## BaseClient

class BaseClient (GenericClient):
    '''
    Base client wrapper for VirtualFileSystem gRPC service.

    Methods in this module are simple wrappers around corresponding gRPC calls,
    whose responses are returned unmodified. Depending on the gRPC channel type,
    this may be an `asyncio` coroutine that must be awaited.

    Likely you will not use this module directly, but rather one of the derived
    classes `Client`, `SignalClient`, `AsyncClient`, or `AsyncSignalClient`.
    '''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `cc.messaging.grpc.GenericClient` base to instantiate `self.stub`.
    from .vfs_service_pb2_grpc import VirtualFileSystemStub as Stub

    ## Ensure that methods overridden in subclasses inherit the docstrings
    ## below.
    # def __init_subclass__(subclass, **kwargs):
    #     super().__init_subclass__(**kwargs)
    #     BaseClient.inherit_docstrings(subclass)

    def get_contexts(self,
                     open_only: bool = False,
                     removable_only: bool = False,
                     ) -> ContextMap:
        '''
        List available virtual filesystem contexts.

        @param removable_only
            include only removable contexts in repsonse

        @param open_only
            include only contexts currently held open

        @returns
            A ContextMap instance, whose sole attribute `map`
            is a mapping of context names to corresponding specs.
        '''

        req = GetContextsRequest(
            open_only = open_only,
            removable_only = removable_only,
        )

        return self.stub.GetContexts(req)

    def get_context(self, name: str) -> ContextSpec:
        '''
        List available virtual filesystem contexts.
        '''
        return self.stub.GetContextSpec(VFSPath(context=name))


    def open_context(self, name: str) -> ContextSpec:
        '''
        Explicitly open a virtual context. Internally this increments a
        reference counter to keep the corresponding virtual filesystem context
        (VFC) open.  For instance, in the case of a removable or remote drive,
        the drive will remain mounted, thereby eliminating overhead associated
        with mounting/unmounting for individual file transactions.

        Once access is no longer needed The client should invoke
        'close_context()', thereby allowing the context to be closed.
        '''

        return self.stub.OpenContext(VFSPath(context=name))


    def close_context(self, name: str):
        '''
        Close a previously-opened path. Internally, this decrements a
        reference counter associated with the underlying filesystem context,
        and closes the context (e.g. unmounts) if it reaches zero.
        '''

        return self.stub.CloseContext(VFSPath(context=name))


    def get_volume_info(self,
                        vfspath: VFSPathInput,
                        dereference: bool =False) -> VolumeInfo:
        '''
        Return information about mounted filesystem containing path.
        '''
        return self.stub.GetVolumeInfo(
            pathRequest(vfspath, dereference=dereference))


    def get_file_info(self,
                      vfspath: VFSPathInput,
                      dereference: bool = True,
                      with_attributes: bool = True) -> FileInfo:
        '''
        Return file stats for specified VFS path, specified in the format
        CONTEXT:RELPATH.

        The returned value is a 'FileInfo()` named tuple, and includes the
        following attributes:
         - `name`        : Base name, without leading directory
         - `type`        : FILE, DIRECTORY, etc
         - `size`        : Size in bytes
         - `link`        : Target for symbolic links
         - `mode`        : UNIX mode mask
         - `readable`    : Boolean indicating readable file/listable directory
         - `writable`    : Boolean indicating writable file/modifiable directory
         - `uid`         : Owner numeric ID
         - `gid`         : Group numeric ID
         - `ownername`   : Owner name
         - `groupname`   : Group name
         - `access_time` : Last access
         - `modify_time` : Last modification
         - `create_time` : Creation time
         - `attributes`  : Dictionary of custom attributes
        '''

        request = pathRequest(path=vfspath,
                              dereference=dereference,
                              with_attributes=with_attributes)
        return self.stub.GetFileInfo(request)


    def get_directory(self,
                      vfspath: VFSPathInput,
                      dereference: bool = True,
                      with_attributes: bool = True) -> Directory:
        '''
        List contents of the specified virtual path, specified in the format
        CONTEXT:RELPATH.

        The returned value is a Python dictionary where keys represent the
        name of each entry and the associated value is a FileInfo instance
        containing detailed information about the file. See `get_file_info()`
        for more info.
        '''

        request = pathRequest(path = vfspath,
                              dereference = dereference,
                              with_attributes = with_attributes)
        return self.stub.GetDirectory(request)


    @abstractmethod
    def list(self,
             vfspath: VFSPathInput,
             dereference: bool = True) -> Sequence[str]:
        '''
        List contents of the specified virtual path, specified in the format
        CONTEXT:RELPATH.

        @param vfspath
            Virtual path in the format CONTEXT:RELPATH.

        @param dereference
            Follow symlinks to include information about the underlying file.

        The returned value is a Python list containing the virtual path of each
        item in the specified folder.
        '''

        request = pathRequest(path = vfspath,
                              dereference = dereference,
                              with_attributes = False)
        return self.stub.GetDirectory(request)


    def locate(self,
               vfspath: VFSPathInput,
               filename_masks: VFSPathInputs,
               attribute_filters: dict = {},
               with_attributes: bool = True,
               include_hidden: bool = False,
               ignore_case: bool = False) -> Directory:
        '''
        Recursively locate file(s) matching the specificed filename masks
        and attribute values.

        @param vfspath
            Folder below which to search

        @param filename_masks
            A string or tuple of strings with glob-style pattners (like "*.txt")

        @param attribute_filters
            A dictionary of attribute patterns to match

        @param with_attributes
            Include custom attributes in response

        @param include_hidden
            Also match leading "." in filename expansion patterns like "*".
            (Normally such filenames need to be matched specifically, i.e. ".*").

        @param ignore_case
            Case insensitive filename matching.

        @returns
            Discovered file paths mapped to corresponding file information.
        '''

        return self.stub.Locate(
            locateRequest(vfspath, filename_masks, attribute_filters,
                          with_attributes, include_hidden, ignore_case))


    def copy(self,
             sources: str|Sequence[str],
             target: str,
             force: bool = False,
             dereference: bool = False,
             merge: bool = False,
             update: bool = False,
             with_attributes: bool = True,
             inside_target: bool = False):
        '''
        Copy the specified source path(s) to the specified target path.

        @param sources
            A string containing a single path, or a tuple or list with multiple
            paths. In the latter case, the option 'inside_target' must also be
            set; as a precaution, this is not automatically assumed.  Each path
            is specified in the format `CONTEXT:RELPATH`.

        @param target
            The final target path to be created or replaced. However if
            'inside_target' is set, 'target' specifies a folder into which the
            source path(s) will be copied.

        @param force
            Replace any existing target path(s) by the same name. Additionally,
            any missing parent folders leading up to the target are implicitly
            created.

        @param dereference
            Follow any symbolic links on source or target. However, note that
            links are never followed outside of the context.  Be careful with
            this option in conjunction with 'force'.

        @param merge
            Merge source folders onto corresponding folders on target, rather
            than entirely replacing the latter.

        @param update
            Only copy paths that are newer than the source than on the target.

        @param with_attributes
            Copy custom VFS attributes associated with the file(s).

        @param inside_target
            Target specifies the parent directory rather than the target file.
        '''

        return self.stub.Copy(
            pathRequest(path=target,
                        sources=sources,
                        force=force,
                        dereference=dereference,
                        merge=merge,
                        update=update,
                        with_attributes=with_attributes,
                        inside_target=inside_target))

    def move(self,
             sources: str|Sequence[str],
             target: str,
             force: bool = False,
             dereference: bool = False,
             merge: bool = False,
             update: bool = False,
             with_attributes: bool = True):
        '''
        Move the specified source path(s) to the specified target path.

        @param sources
            A string containing a single path, or a tuple or list with multiple
            paths. In the latter case, the option 'inside_target' must also be
            set; as a precaution, this is not automatically assumed.  Each path
            is specified in the format `CONTEXT:RELPATH`.

        @param target
            The final target path to be created or replaced. However if
            'inside_target' is set, 'target' specifies a folder into which the
            source path(s) will be moved.

        @param force
            Replace any existing target path(s) by the same name. Additionally,
            any missing parent folders leading up to the target are implicitly
            created.

        @param dereference
            Follow any symbolic links on source or target. However, note that
            links are never followed outside of the context.  Be careful with
            this option in conjunction with 'force'.

        @param merge
            Merge source folders into corresponding folders on target, rather
            than entirely replacing the latter.

        @param update
            Only move paths that are newer than the source than on the target.

        @param with_attributes
            Move (remove on soure, add on target) any custom VFS attributes
            associated with the file(s).
        '''
        return self.stub.Move(
            pathRequest(path=target,
                        sources=sources,
                        force=force,
                        dereference=dereference,
                        merge=merge,
                        update=update,
                        with_attributes=with_attributes))



    def create_folder(self,
                      vfspath: VFSPathInput,
                      force: bool = False,
                      dereference: bool = False):
        '''
        Create a folder

        @param vfspath
            Target folder in the format CONTEXT:RELPATH.

        @param force
            Create any missing parent folders leading up to the target folder.
            Any non-directory components in the way are silently removed.

        @param dereference
            Follow any symbolic links within the target path. However, note that
            links are never followed outside of the context.  Be careful with
            this option in conjunction with 'force'.
        '''
        return self.stub.CreateFolder(
            pathRequest(path=vfspath,
                        force=force,
                        dereference=dereference))


    def remove(self,
               paths: VFSPathInputs,
               force: bool = False,
               dereference: bool = False,
               with_attributes: bool = True):
        '''
        Remove one or more paths.

        @param paths
            A string containing a single path, or a tuple or list with multiple
            paths, each in the format `CONTEXT:RELPATH`.

        @param force
            Recursively remove paths even if they are on-empty directories.
            Additionally, do not complain about any missing paths.

        @param dereference
            Follow any symbolic links within the target path. However, note that
            links are never followed outside of the context.  Be careful with
            this option in conjunction with 'force'.

        @param with_attributes
            Also remove any custom VFS attributes associated with the removed
            path(s).
        '''
        return self.stub.Remove(
            pathRequest(sources=paths,
                        force=force,
                        dereference=dereference,
                        with_attributes=with_attributes))

    def read(self, vfspath: VFSPathInput) -> Iterator[FileChunk]:
        '''
        Read from a file on the server.

        @param vfspath
            File path, specified in the format CONTEXT:RELPATH.

        @returns
            An iterator over byte chunks from the requested file.

        See also `read_file()` if you want to iterate over the underlying
        `cc.protobuf.vfs.FileChunk` instances, or `download()` if you just want
        to download a file from the server and save onto the local fileysstem.
        '''

        return self.read_file(vfspath)


    def read_file(self, vfspath: VFSPathInput) -> Iterator[FileChunk]:
        '''
        Read from a file on the server.

        @param vfspath
            File path, specified in the format CONTEXT:RELPATH.

        @returns
             A gRPC ClientReader instance, which can be used to iterate over
             `cc.protobuf.vfs.FileChunk` instances.

        See also `read()` if you want to iterate over just the file contents, or
        `download()` if you want to download and save a file from the server to
        a local file.
        '''
        return self.stub.ReadFile(encodePath(vfspath))


    def download(self,
                 vfspath: VFSPathInput,
                 localfile: LocalFile):
        '''
        Download a file from the server onto the local filesystem.

        @param vfspath
            File path, specified in the format CONTEXT:RELPATH.

        @param localfile
            Either a local (host native) file path, relative to the client's
            current working directory, or a writable file object (with a
            compatible `write()` method).
        '''

        if isinstance(localfile, pathlib.Path):
            localfile = localfile.open("wb")

        elif isinstance(localfile, str):
            localfile = open(localfile, "wb")

        for chunk in self.read_file(vfspath):
            localfile.write(chunk.data)


    def upload(self,
               localfile: LocalFile,
               vfspath: VFSPathInput):
        '''
        Upload the contents of `localfile` onto the server.

        @param localfile
            Either an existing local (host native) file path, relative to the
            client's current working directory, or a readable file object (with
            a compatible `read1()` method).

        @param vfspath
            Server file path to create or replace, specified in the format
            CONTEXT:RELPATH.
        '''

        path = encodePath(vfspath)

        if isinstance(localfile, pathlib.Path):
            localfile = localfile.open("rb")

        elif isinstance(localfile, str):
            localfile = open(localfile, "rb")

        return self.stub.WriteFile(
            self._uploadIterator(localfile, vfspath))


    def _uploadIterator(self, fp, vfspath):
        '''
        Internal: Iterate over contents of a file and generate FileChunk
        messages
        '''

        path = encodePath(vfspath)
        while data := fp.read1():
            yield FileChunk(path=path, data=data)
            path = None

    def get_attributes(self, vfspath: VFSPathInput) -> KeyValueMap:
        '''
        Returns a dictionary of attributes associated with the specified path

        @param vfspath
            Virtual path on the server in the format CONTEXT:RELPATH.

        @returns
            Key/value pairs representing custom attributes associated with the
            specified path.
        '''

        req = encodePath(vfspath)
        return self.stub.GetAttributes(req)

    def set_attributes(self, vfspath: VFSPathInput, **attributes):
        '''
        Add/update custom attributes associated with the specified path.

        @param vfspath
            Virtual path on the server in the format CONTEXT:RELPATH.

        @param attributes
            Arbitrary key/value pairs to add/replace as custom atrbutes on the
            specified path.
        '''

        self.stub.SetAttributes(attributeRequest(vfspath, attributes))

    def clear_attributes(self, vfspath: VFSPathInput):
        '''
        Clear all custom attributes associated with a path on the server.

        @param vfspath
            Virtual path on the server in the format CONTEXT:RELPATH.
        '''

        return self.stub.ClearAttributes(encodePath(vfspath))
