// -*- c++ -*-
//==============================================================================
/// @file vfs.h++
/// @brief VFS service - common API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-base.h++"

namespace vfs
{
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
    ContextMap get_contexts(bool removable_only = false, bool open_only = false);

    /// Get a context reference without incrementing reference counter.
    ContextMap getRemovableContexts();

    /// Get a context reference without incrementing reference counter.
    ContextMap get_open_context(bool removable = false);

    /// Get a context reference without incrementing reference counter.
    Context::ptr get_context(const std::string &name, bool required = true);

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
    /// `close_context()', thereby allowing the context to be closed.
    Context::ptr open_context(const std::string &name, bool required = true);

    /// Close a previously-opened path. Internally, this decrements a
    /// reference counter associated with the underlying filesystem context,
    /// and closes the context (e.g. unmounts) if it reaches zero.
    void close_context(const Context::ptr &cxt);

    /// Close a previously-opened path. Internally, this decrements a
    /// reference counter associated with the underlying filesystem context,
    /// and closes the context (e.g. unmounts) if it reaches zero.
    void close_context(const std::string &name);

    /// Check if a given path exist
    bool exists(const Path &vpath);

    /// Return information about mounted filesystem containing path.
    VolumeInfo get_volume_info(
        const Path &vpath);

    /// Return status of specified paths.
    /// Unlike `list()` this does not descent into folders.
    FileInfo get_file_info(
        const Path &vpath,
        bool with_attributes = true,
        bool dereference = true);

    /// List contents of the specified virtual path (context+folder).
    Directory get_directory(
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

    /// @brief
    ///    Recursively locate files matching specific patterns and/or attributes.
    /// @param[in] root
    ///    Top-level folder in which to start the search
    /// @param[in] filename_masks
    ///    Zero or more glob-style file masks, e.g. `*.txt`.
    /// @param[in] attribute_filters
    ///    Zero or more tag/value pairs that must be present.
    ///    Tags may be repeated for multiple cumulative value matches.
    /// @param[in] with_attributes
    ///    Include custom attributes in response
    /// @param[in] include_hidden
    ///     Also match leading "." in filename expansion patterns like "*".
    ///     (Normally such filenames need to be matched specifically, i.e. ".*").
    /// @param[in] ignore_case
    ///     Case insensitive filename matching

    Directory locate(
        const Path &root,
        const std::vector<fs::path> &filename_masks,
        const core::types::TaggedValueList &attribute_filters = {},
        bool with_attributes = true,
        bool include_hidden = false,
        bool ignore_case = false);

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

    /// @brief Move `source` path to `target.
    /// @param[in] source
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
    /// @param[in] inside_target
    ///     Indicates that `target` is not the final target path, but an
    ///     existing folder into which the source path will be copied.
    void move(
        const Path &source,
        const Path &target,
        bool force = false,
        bool with_attributes = true,
        bool inside_target = false);

    /// @brief Move zero or more source path(s) inside specified target folder.
    /// @param[in] sources
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
    void move(
        const Paths &sources,
        const Path &target,
        bool force = false,
        bool with_attributes = true);

    /// @brief Remove file or folder
    /// @param[in] path
    ///     File location to be removed
    /// @param[in] force
    ///     Attempts to remove the target even if
    ///       - It is a non-empty folder
    ///       - It is not writable (and in the case of folders, executable)
    /// @param[in] with_attributes
    ///     Also remove attributes associated with file.
    void remove(
        const Path &path,
        bool force = false,
        bool with_attributes = true);

    /// @brief Remove file or folder
    /// @param[in] paths
    ///     File location to be removed
    /// @param[in] force
    ///     Attempts to remove the target even if
    ///       - It is a non-empty folder
    ///       - It is not writable (and in the case of folders, executable)
    /// @param[in] with_attributes
    ///     Also remove attributes associated with each file.
    void remove(
        const Paths &paths,
        bool force = false,
        bool with_attributes = true);

    /// @brief Create a new folder
    /// @param[in] vpath
    ///     VFS location of the new folder
    /// @param[in] force
    ///     Enables the following behaviors
    ///       - If the parent folder does not exist, create it
    ///       - Do not raise error if the folder already exists
    void create_folder(
        const Path &vpath,
        bool force = false);

    /// @brief Read the contents of file from the server
    /// @param[in] vpath
    ///     Path to new folder
    /// @return
    ///     Unique pointer to a FileReader instance, which in turn is
    ///     dervived from `std::istream`.

    UniqueReader read_file(const Path &vpath);
    UniqueWriter write_file(const Path &vpath);

    void download(const Path &remote, const fs::path &local);

    void upload(const fs::path &local, const Path &remote);

    core::types::KeyValueMap get_attributes(const Path &vpath);
    core::types::Value getAttribute(const Path &vpath,
                                    const std::string attribute,
                                    const core::types::Value fallback = {});
    void set_attributes(const Path &vpath,
                        const core::types::KeyValueMap &attributes);

    void setAttribute(const Path &vpath,
                      const std::string &key,
                      const core::types::Value &value);

    void clear_attributes(const Path &vpath);
}  // namespace vfs
