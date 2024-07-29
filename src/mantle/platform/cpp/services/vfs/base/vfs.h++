// -*- c++ -*-
//==============================================================================
/// @file vfs.h++
/// @brief VFS service - common API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-base.h++"

namespace platform::vfs
{
    /// Open a new ContextProxy object to access the specified context.
    ContextProxy contextProxy(const std::string &name, bool modify);

    /// Open a new ContextProxy object to access the specified context.
    ContextProxy contextProxy(ContextRef ref, bool modify);

    /// Open a new location object to access the specified virtual path
    Location location(const Path &vpath, bool modify);

    /// Open a new Location object to access the specified context/path.
    Location location(const std::string &context, const fs::path &relpath, bool modify);

    /// Open a new Location object to access the specified context instance/path.
    Location location(ContextRef ref, const fs::path &relpath, bool modify);

    /// Open multiple locations, possibly within different contexts.
    LocationList locations(const Paths &vpaths, bool modify);

    /// Obtain local path to the specified virtual path.
    fs::path localPath(const Path &vpath);

    /// Obtain virtual path of the parent direcory. Similar to fs::remove_filename
    Path getParentPath(const Path &vpath);

    /// List available virtual filesystem contexts.
    /// If empty, lists available virtual filesystem contexts.
    ContextList listContexts();

    /// @brief Get map of available virtual filesystem contexts.
    /// @param[in] removable_only
    ///    Return only removable contexs
    /// @param[in] open_only
    ///    Return only open contexts
    /// @return
    ///    Vector of contexts IDs and corresponding instances
    ContextMap getContexts(bool removable_only = false, bool open_only = false);

    /// Get a context reference without incrementing reference counter.
    ContextMap getRemovableContexts();

    /// Get a context reference without incrementing reference counter.
    ContextMap getOpenContexts(bool removable = false);

    /// Get a context reference without incrementing reference counter.
    ContextRef getContext(const std::string &name, bool required = true);

    /// @brief Explicitly open a virtual context.
    /// @param[in] name
    ///    Context name
    /// @param[in] required
    ///    If the context does not exist, throw an exception
    ///    (`exception::NotFound`). Otherwise an empty pointer is returned.
    ///
    /// Internally this increments a reference counter to keep the corresponding
    /// virtual filesystem context (VFC) open.  For instance, in the case of a
    /// removable or remote drive, the drive will remain mounted, thereby
    /// eliminating overhead associated with mounting/unmounting for individual
    /// file transactions.
    ///
    /// Once access is no longer needed The client should invoke
    /// `closeContext()', thereby allowing the context to be closed.
    ContextRef openContext(const std::string &name, bool required = true);

    /// Close a previously-opened path. Internally, this decrements a
    /// reference counter associated with the underlying filesystem context,
    /// and closes the context (e.g. unmounts) if it reaches zero.
    void closeContext(const ContextRef &cxt);

    /// Close a previously-opened path. Internally, this decrements a
    /// reference counter associated with the underlying filesystem context,
    /// and closes the context (e.g. unmounts) if it reaches zero.
    void closeContext(const std::string &name);

    /// Check if a given path exist
    bool exists(const Path &vpath);

    /// Return information about mounted filesystem containing path.
    VolumeStats volumeStats(
        const Path &vpath);

    /// Return status of specified paths.
    /// Unlike `list()` this does not descent into folders.
    FileStats fileStats(
        const Path &vpath,
        bool with_attributes = true,
        bool dereference = true);

    /// List contents of the specified virtual path (context+folder).
    Directory getDirectory(
        const Path &vpath,
        bool with_attributes = true,
        bool dereference = true,
        bool include_hidden = false);

    /// List contents of the specified virtual path (context+folder).
    DirectoryList list(
        const Path &vpath,
        bool with_attributes = true,
        bool dereference = true,
        bool include_hidden = false);

    /// Recursively locate files matching specific patterns and/or attributes.
    /// @param[in] root
    ///    Top-level folder in which to start the search
    /// @param[in] filename_masks
    ///    Zero or more glob-style file masks, e.g. `*.txt`.
    /// @param[in] attribute_filters
    ///    Zero or more tag/value pairs that must be present.
    ///    Tags may be repeated for multiple cumulative value matches.

    Directory locate(
        const Path &root,
        const std::vector<std::string> &filename_masks,
        const core::types::TaggedValueList &attribute_filters = {},
        bool with_attributes = true,
        bool include_hidden = false,
        bool ignore_case = false);

    /// @fn copy
    /// @brief Copy `source` file or folder.
    /// @param[in] source
    ///     source path
    /// @param[in] target
    ///     target path. Will be created/replaed unless `inside_target` is specified.
    /// @param[in] force
    ///     Enables the following behaviors:
    ///       - Recursively copy/move/remove a folder and its children
    ///       - Replaces an existing target regardless of type
    ///       - Copies metadata from source, even if replacing existing target
    ///       - Creates missing parent folder(s) on target
    /// @param[in] dereference
    ///     If source or target is a symbolic link, follow it.
    /// @param[in] merge
    ///     Merge rather than replace an existing target folder.
    ///     Effective only if both `source` and `target` are folders.
    /// @param[in] update
    ///     Copy the file only if it is newer on `source`.
    /// @param[in] with_attributes
    ///     Copy/replace attributes on target from source
    /// @param[in] inside_target
    ///     Indicates that `target` is not the final target path, but an
    ///     existing folder into which the source path will be copied.

    void copy(
        const Path &source,
        const Path &target,
        bool force = false,
        bool dereference = false,
        bool merge = false,
        bool update = false,
        bool with_attributes = true,
        bool inside_target = false);

    /// @fn copy
    /// @brief Copy zero or more path(s) inside a target folder
    /// @param[in] sources
    ///     source paths
    /// @param[in] target
    ///     target folder into which the source(s) will be copied
    /// @param[in] force
    ///     Enables the following behaviors:
    ///       - Recursively copy/move/remove a folder and its children
    ///       - Replaces any existing target regardless of type
    ///       - Copies metadata from source, even if replacing existing target
    ///       - Creates missing parent folder(s) on target
    /// @param[in] dereference
    ///     If source or target is a symbolic link, follow it.
    /// @param[in] merge
    ///     Merge rather than replace an existing target folder.
    ///     Effective only if both `source` and `target` are folders.
    /// @param[in] update
    ///     Copy the file only if it is newer on `source`.
    /// @param[in] with_attributes
    ///     Copy/replace attributes on target from source

    void copy(
        const Paths &sources,
        const Path &target,
        bool force = false,
        bool dereference = false,
        bool merge = false,
        bool update = false,
        bool with_attributes = true);

    /// @fn move
    /// @brief Move `source` path to `target.
    /// @param[in] sources
    ///     source path
    /// @param[in] target
    ///      target path. Will be created/replaed unless `inside_target` is specified.
    /// @param[in] force
    ///     Enables the following behaviors:
    ///       - Recursively copy/move/remove a folder and its children
    ///       - Replaces an existing target regardless of type
    ///       - Copies metadata from source, even if replacing existing target
    ///       - Creates missing parent folder(s) on target
    /// @param[in] with_attributes
    ///     Copy/replace attributes on target from source
    /// @param[in]
    ///     Merge rather than replace an existing target folder.
    ///     Effective only if both `source` and `target` are folders.
    /// @param[in] inside_target
    ///     Indicates that `target` is not the final target path, but an
    ///     existing folder into which the source path will be copied.
    void move(
        const Path &source,
        const Path &target,
        bool force = false,
        bool with_attributes = true,
        bool inside_target = false);

    /// @fn move
    /// @brief Move zero or more source path(s) inside specified target folder.
    /// @param[in] source
    ///     source path
    /// @param[in] target
    ///     target folder into which the source(s) will be moved
    /// @param[in] force
    ///     Enables the following behaviors:
    ///       - Recursively copy/move/remove a folder and its children
    ///       - Replaces an existing target regardless of type
    ///       - Copies metadata from source, even if replacing existing target
    ///       - Creates missing parent folder(s) on target
    /// @param[in] with_attributes
    ///     Copy/replace attributes on target from source
    /// @param[in]
    ///     Merge rather than replace an existing target folder.
    ///     Effective only if both `source` and `target` are folders.
    void move(
        const Paths &sources,
        const Path &target,
        bool force = false,
        bool with_attributes = true);

    /// @fn remove
    /// @brief Remove file or folder
    /// @param[in] source
    ///     File location to be removed
    /// @param[in] force
    ///     Attempts to remove the target even if
    ///       - It is a non-empty folder
    ///       - It is not writable (and in the case of folders, executable)
    void remove(
        const Path &source,
        bool force = false,
        bool with_attributes = true);

    void remove(
        const Paths &sources,
        bool force = false,
        bool with_attributes = true);

    /// @fn createFolder
    /// @brief Create a new folder
    /// @param[in] loc
    ///     VFS location of the new folder
    /// @param[in] force
    ///     Enables the following behaviors
    ///       - If the parent folder does not exist, create it
    ///       - Do not raise error if the folder already exists
    void createFolder(
        const Path &vpath,
        bool force = false);

    /// @fn readFile
    /// @brief Read the contents of file from the server
    /// @param[in] vpath
    ///     Path to new folder
    /// @return
    ///     Unique pointer to a FileReader instance, which in turn is
    ///     dervived from `std::istream`.

    ReaderRef readFile(const Path &vpath);
    WriterRef writeFile(const Path &vpath);

    void download(const Path &remote, const fs::path &local);

    void upload(const fs::path &local, const Path &remote);

    core::types::KeyValueMap getAttributes(const Path &vpath);
    core::types::Value getAttribute(const Path &vpath,
                                    const std::string attribute,
                                    const core::types::Value fallback = {});
    void setAttributes(const Path &vpath,
                       const core::types::KeyValueMap &attributes);

    void setAttribute(const Path &vpath,
                      const std::string &key,
                      const core::types::Value &value);

    void clearAttributes(const Path &vpath);
}  // namespace platform::vfs
