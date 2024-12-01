// -*- c++ -*-
//==============================================================================
/// @file vfs-location.h++
/// @brief VFS service - abstract file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-context.h++"

namespace platform::vfs
{
    //==========================================================================
    /// @class ContextProxy
    /// @brief Proxy to use as local reference for LocalContext.
    ///     Adds reference on creation, removes when out of scope. The
    ///     contexts's pre-exec hook is executed when the first reference
    ///     is added, and the post-exec hook is executed when no remaining
    ///     references exist.

    class ContextProxy : public core::types::Streamable
    {
    public:
        ContextProxy();
        ContextProxy(Context::ptr context, bool modify);
        ContextProxy(const ContextProxy &other);
        ~ContextProxy();

        operator bool() const noexcept;
        Context *operator->() const;

    private:
        void add_ref();
        void del_ref();
        void check_access(bool modify) const;
        void check_modify_access() const;

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        virtual fs::path localPath(const fs::path &relpath = {}) const;
        virtual Path virtualPath(const fs::path &relpath = {}) const;

    public:
        Context::ptr context;
        bool modify;
    };

    //==========================================================================
    /// @class Location
    /// @brief Reference to a specifc file/location within a VFS context.

    class Location : public ContextProxy
    {
        using This = Location;
        using Super = ContextProxy;

    public:
        Location();

        Location(Context::ptr context,
                 const fs::path &relpath,
                 bool modify);

        Location(Context::ptr context,
                 bool modify);

        Location(const Location &other);

        virtual fs::path localPath() const;
        fs::path localPath(const fs::path &relpath) const override;

        virtual Path virtualPath() const;
        Path virtualPath(const fs::path &relpath) const override;

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        // virtual VolumeInfo get_volume_info(
        //     const OperationFlags &flags = {}) const = 0;

        // virtual FileInfo get_file_info(
        //     const OperationFlags &flags = {}) const = 0;

        // virtual Directory get_directory(
        //     const OperationFlags &flags = {}) const = 0;

        // virtual Directory locate(
        //     const std::vector<std::string> &filename_masks,
        //     const core::types::TaggedValueList &attribute_filters = {},
        //     const OperationFlags &flags = {}) const = 0;

        // virtual void copy_from(
        //     const LocationList &sources,
        //     const OperationFlags &flags = {}) const = 0;

        // virtual void move_from(
        //     const LocationList &sources,
        //     const OperationFlags &flags = {}) const = 0;

        // virtual void remove(
        //     const OperationFlags &flags = {}) const = 0;

        // virtual void create_folder(
        //     const OperationFlags &flags) const = 0;

        // virtual UniqueReader read() const = 0;

        // virtual UniqueWriter write() const = 0;

        // virtual core::types::KeyValueMap get_attributes() const = 0;

        // virtual void set_attributes(
        //     const core::types::KeyValueMap &attributes,
        //     bool save = true) const = 0;

        // virtual void clear_attributes(
        //     bool save = true) const = 0;

    public:
        fs::path relpath;
    };

    using LocationList = std::vector<Location>;
}  // namespace platform::vfs
