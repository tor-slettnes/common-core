#!/usr/bin/python3 -i
#===============================================================================
## @file client.py
## @brief Python client for `VirtualFileSystem` gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.messaging.grpc import SignalClient, DetailedError
from cc.protobuf.import_proto import import_proto
from cc.protobuf.vfs import VFSPathType, VFSPathsType, \
    encodePath, encodePaths, decodeStats, \
    pathRequest, locateRequest, attributeRequest

### Third-party modules
from google.protobuf.empty_pb2 import Empty

### Standard Python modules
from typing import Mapping
import io

## Import generated ProtoBuf symbols. These will appear in namespaces
## corresponding to the package names from their `.proto` files:
## `google.protobuf` and `cc.platform.vfs`.
import_proto('vfs', globals())

#===============================================================================
## VirtualFileSystemClient

class VirtualFileSystemClient (SignalClient):
    '''Client for VirtualFileSystem service.'''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `messaging.grpc.Client` base to instantiate `self.stub`.
    from cc.generated.vfs_pb2_grpc import VirtualFileSystemStub as Stub

    ## `signal_type` is used to construct a `cc.protobuf.SignalStore` instance,
    ## which serves as a clearing house for emitting and receiving messages.
    signal_type = cc.platform.vfs.Signal

    Signals = (SIGNAL_CONTEXT, SIGNAL_CONTEXT_IN_USE) \
        = ('context', 'context_in_use')


    def get_contexts(self,
                    removable_only: bool = False,
                    open_only: bool = False) -> Mapping[str, cc.platform.vfs.ContextSpec]:
        '''
        List available virtual filesystem contexts.
        @param[in] removable_only
            include only removable contexts in repsonse
        @param[in] open_only
            include only contexts currently held open
        @return
            List of `(name, cxt)` tuples.
        '''
        if open_only:
            data = self.get_open_context()
        else:
            data = self.getAllContexts()

        if removable_only:
            data = [ (name, cxt)
                     for (name, cxt) in data.items()
                     if cxt.removable ]
        return dict(data)

    def get_removable_contexts(self,
                               open_only: bool = False) -> Mapping[str, cc.platform.vfs.ContextSpec]:
        '''
        List filesystem contexts that map to removable drives
        '''
        return self.get_contexts(True, open_only)

    def get_all_contexts(self) -> Mapping[str, cc.platform.vfs.ContextSpec]:
        '''
        List all available VFS contexts.
        '''
        return self.stub.get_contexts(Empty()).map

    def get_open_context (self) -> Mapping[str, cc.platform.vfs.ContextSpec]:
        '''
        List VFS contexts that are currently being held open.
        '''
        return self.stub.get_open_context(Empty()).map


    def get_context(self, name: str) -> cc.platform.vfs.ContextSpec:
        '''
        List available virtual filesystem contexts.
        '''
        return self.stub.get_context_spec(cc.platform.vfs.Path(context=name))


    def open_context(self, name: str) -> cc.platform.vfs.ContextSpec:
        '''
        Explicitly open a virtual context. Internally this increments a
        reference counter to keep the corresponding virtual filesystem
        context (VFC) open.  For instance, in the case of a removable or
        remote drive, the drive will remain mounted, thereby eliminating
        overhead associated with mounting/unmounting for individual file
        transactions.

        Once access is no longer needed The client should invoke
        'close_context()', thereby allowing the context to be closed.
        '''

        return self.stub.open_context(cc.platform.vfs.Path(context=name))


    def close_context(self, name: str):
        '''
        Close a previously-opened path. Internally, this decrements a
        reference counter associated with the underlying filesystem context,
        and closes the context (e.g. unmounts) if it reaches zero.
        '''

        return self.stub.close_context(cc.platform.vfs.Path(context=name))


    def get_volume_info(self,
                     vfspath: VFSPathType,
                     dereference: bool =False) -> cc.platform.vfs.VolumeInfo:
        '''
        Return information about mounted filesystem containing path.
        '''
        return self.stub.get_volume_info(
            pathRequest(vfspath, dereference=dereference))


    def get_file_info(self,
                  vfspath: VFSPathType,
                  dereference: bool = True,
                  with_attributes: bool = True) -> cc.platform.vfs.FileInfo:
        '''
        Return file stats for specified VFS path, specified in the format
        CONTEXT:PATH.

        The returned value is a 'FileInfo()` named tuple, and includes
        the following attributes:
           name       : Base name, without leading directory
           type       : TYPE_FILE, TYPE_DIRECTORY, etc
           size       : Size in bytes
           link       : Target for symbolic links
           mode       : UNIX mode mask
           readable   : Boolean indicating readable file/listable directory
           writable   : Boolean indicating writable file/modifiable directory
           uid        : Owner numeric ID
           gid        : Group numeric ID
           ownername  : Owner name
           groupname  : Group name
           accessTime : Last access
           modifyTime : Last modification
           createTime : Creation time
           attributes : Dictionary of custom attributes
        '''

        request = pathRequest(source=vfspath,
                              dereference=dereference,
                              with_attributes=with_attributes)
        reply = self.stub.get_file_info(request)
        return decodeStats(reply)


    def get_directory(self,
                      vfspath: VFSPathType,
                      dereference: bool = True,
                      with_attributes: bool = True) -> Mapping[str, cc.platform.vfs.FileInfo]:
        '''
        List contents of the specified virtual path, specified in the format
        CONTEXT:PATH.

        The returned value is a Python dictionary where keys represent the
        name of each entry and the associated value is a FileInfo instance
        containing detailed information about the file. See `get_file_info()`
        for more info.
        '''

        reply = self.stub.get_directory(
            pathRequest(source=vfspath,
                        dereference=dereference,
                        with_attributes=with_attributes))
        entries = [ (key, decodeStats(stats))
                    for (key, stats) in reply.map.items() ]
        return dict(entries)


    def locate(self,
                vfspath: VFSPathType,
                filename_masks: VFSPathsType,
                attribute_filters: dict = {},
                with_attributes: bool = True,
                include_hidden: bool = False) -> Mapping[str, cc.platform.vfs.FileInfo]:
        '''
        Recursively locate file(s) matching the specificed filename masks
        and attribute values.

        @param[in] filename_masks
            a string or tuple of strings with glob-style pattners (like "*.txt")

        @param[in] attribute_filters
            a dictionary of attribute patterns to match

        @param[in] with_attributes
            Include custom attributes in response

        @param[in] include_hidden
            Also match leading "." in filename expansion patterns like "*".
            (Normally such filenames need to be matched specifically, i.e. ".*").
        '''

        reply = self.stub.locate(
            locateRequest(vfspath, filename_masks, attribute_filters,
                          with_attributes, include_hidden))

        entries = [ (key, decodeStats(stats))
                    for (key, stats) in reply.map.items() ]

        return dict(entries)


    def copy(self,
              source: str,
              target: str,
              force: bool = False,
              dereference: bool = False,
              merge: bool = False,
              update: bool = False,
              with_attributes: bool = True,
              inside_target: bool = False):
        '''
        Copy the specified source path(s) to the specified target path.
        Each path is specified in the format CONTEXT:RELPATH, where
           CONTEXT is a predefined, possibly removable location within the filesystem
           RELPATH is a path relative to the context's root.

        The 'source' argument may be a string containing a single path, or a
        tuple or list with multiple paths. In the latter case, the option
        'inside_target' must also be set.

        By default 'target' specifies the final target path to be created or
        replaced. However if 'inside_target' is set, 'target' specifies a folder
        into which the source path(s) will be copied.

        If 'force' is set, any existing target path(s) will be replaced, and
        any missing parent folders leading up to the target are implicitly created.

        If 'dereference' is set, any symbolic links on source or target will
        be followed.  However, note that links are never followed outside of the
        context.  Be careful with this option in conjunction with 'force'.

        if 'merge' is set, any existing folder(s) on target are merged with
        the corresponding source folder(s) rather than replaced.

        If 'update' is set, any existing paths that are newer than the source
        are not replaced.

        If 'with_attributes' is set (as per default), custom VFS attributes
        are copied alongside the file.
        '''

        return self.stub.copy(
            pathRequest(source=source,
                          target=target,
                          force=force,
                          dereference=dereference,
                          merge=merge,
                          update=update,
                          with_attributes=with_attributes,
                          inside_target=inside_target))

    def move(self,
             source: str,
             target: str,
             force: bool = False,
             dereference: bool = False,
             merge: bool = False,
             update: bool = False,
             with_attributes: bool = True):
        '''
        Move/rename the specified source path(s) to the specified target path.
        Each path is specified in the format CONTEXT:RELPATH, where
           CONTEXT is a predefined, possibly removable location within the filesystem
           RELPATH is a path relative to the context's root.

        The 'source' argument may be a string containing a single path, or a
        tuple or list with multiple paths. In the latter case, the option
        'inside_target' must also be set.

        By default 'target' specifies the final target path to be created or
        replaced. However if 'inside_target' is set, 'target' specifies a folder
        into which the source path(s) will be moved.

        If 'force' is set, any existing target path(s) will be replaced, and
        any missing parent folders leading up to the target are implicitly created.

        If 'dereference' is set, any symbolic links on source or target will
        be followed.  However, note that links are never followed outside of the
        context.  Be careful with this option in conjunction with 'force'.

        if 'merge' is set, any existing folder(s) on target are merged with
        the corresponding source folder(s) rather than replaced.

        If 'update' is set, any existing paths that are newer than the source
        are not replaced.

        If 'with_attributes' is set (as per default), custom VFS attributes
        are moved alongside the file.
        '''
        return self.stub.move(
            pathRequest(source=source,
                        target=target,
                        force=force,
                        dereference=dereference,
                        merge=merge,
                        update=update,
                        with_attributes=with_attributes))



    def create_folder(self,
                     vfspath: VFSPathType,
                     force: bool = False,
                     dereference: bool = False):

        '''Create a folder, specified in the format CONTEXT:PATH.

        If 'force' is specified, missing parent directories are created as
        needed, and any non-directory components in the way are silently
        removed.
        '''
        return self.stub.create_folder(
            pathRequest(target=vfspath,
                        force=force,
                        dereference=dereference))


    def remove(self,
                paths: VFSPathsType,
                force: bool = False,
                dereference: bool = False,
                with_attributes: bool = True):

        '''Remove 'paths', which is either a string containing a single VFS path
        or a tuple or list with multiple paths.  Each path is specified in the
        format CONTEXT:RELPATH, where

        - CONTEXT is a predefined, possibly removable location within the filesystem

        - RELPATH is a path relative to the context's root.

        If the path refers to non-empty directory, an error is returned unless
        'force' is given, in which case it is removed recursively.
        '''
        return self.stub.remove(
            pathRequest(target=paths,
                        force=force,
                        dereference=dereference,
                        with_attributes=with_attributes))


    def read_file(self, vfspath: VFSPathType): ### TODO Type return value
        '''Read a from the file `vfspath` from the server, specified in the format
        CONTEXT:PATH.

        Returns a gRPC ClientReader instance, which can be used to iterate over
        the file contents. in chunk.s
        '''
        return self.stub.read_file(encodePath(vfspath))


    def download(self,
                  vfspath: VFSPathType,
                  localfile: str | io.IOBase):
        '''Download the file `vfspath` from the server, specified in the format
        CONTEXT:PATH.

        Contents are saved to `localfile`, which can either be a local file path
        or a writable file object (or any object with a compatible `write()`
        method).

        '''

        if isinstance(localfile, str):
            localfile = open(localfile, "wb")

        reader = self.read_file(vfspath)
        for chunk in reader:
            print("Writing %d bytes to local file %r"%(len(chunk.data), localfile.name))
            localfile.write(chunk.data)


    def upload(self,
                localfile: str | io.IOBase,
                vfspath: VFSPathType):
        '''Upload the contents of `localfile`, which can either be a local file path or
         a readable file object, to the file `vfspath` on the server, specified
         in the format CONTEXT:PATH.

        '''

        path = encodePath(vfspath)
        if isinstance(localfile, str):
            localfile = open(localfile, "rb")

        return self.stub.write_file(
            self._uploadIterator(localfile, vfspath))


    def _uploadIterator(self, fp, vfspath, chunksize=4096):
        '''Internal: Iterate over contents of a file and generate FileChunk messages'''

        eof = False
        path = encodePath(vfspath)
        while not eof:
            text = fp.read(chunksize)
            if text:
                yield cc.platform.vfs.FileChunk(path=path, data=text)
            else:
                eof = True

    def get_attributes(self, vfspath: VFSPathType) -> Mapping[str, object]:
        '''Returns a dictionary of attributes associated with the specified path'''

        req = encodePath(vfspath)
        resp = self.stub.get_attributes(req)
        return decodeValueMap(resp.items)

    def set_attributes(self, vfspath: VFSPathType, **attributes):
        '''Add/update custom attributes associated with the specified path.'''

        self.stub.set_attributes(attributeRequest(vfspath, attributes))

    def clear_attributes(self, vfspath: VFSPathType):
        '''Clear all custom attributes associated with the specified path.'''

        self.self.stub.clear_attributes(encodePath(vfspath))


if __name__ == '__main__':
    vfs = VirtualFileSystemClient()
