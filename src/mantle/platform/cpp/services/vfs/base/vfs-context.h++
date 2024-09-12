// -*- c++ -*-
//==============================================================================
/// @file vfs-context.h++
/// @brief VFS service - abstract file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-types.h++"

#include <map>
#include <unordered_map>
#include <memory>
#include <iostream>

namespace platform::vfs
{
    //==========================================================================
    /// \class Context
    /// \brief VFS location context.

    class Context : public core::types::Streamable
    {
    public:
        using ptr = std::shared_ptr<Context>;

    public:
        Context(const ContextName &name = {},
                const fs::path &root = {},
                bool writable = false,
                bool removable = false,
                const std::string title = {});

        virtual void add_ref() = 0;
        virtual void del_ref() = 0;

        virtual fs::path localPath(const fs::path &relpath = {}) const;
        virtual Path virtualPath(const fs::path &relpath = {}) const;

    protected:
        virtual void to_stream(std::ostream &stream) const override;

    private:
        void check_jail(const fs::path &relpath) const;

    public:
        ContextName name;
        fs::path root;
        bool writable;
        bool removable;
        std::string title;
    };

    using ContextMap = std::unordered_map<ContextName, Context::ptr>;
    using ContextList = std::vector<Context::ptr>;

    //==========================================================================
    /// \class ContextProxy
    /// \brief Proxy to use as local reference for LocalContext.
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
    /// \class Location
    /// \brief Reference to a specifc file/location within a VFS context.

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

        fs::path localPath(const fs::path &relpath = {}) const override;
        Path virtualPath(const fs::path &relpath = {}) const override;

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        // virtual VolumeStats volume_stats(
        //     const fs::path &relpath,
        //     const OperationFlags &flags = {}) const = 0;

        // virtual FileStats file_stats(
        //     const fs::path &relpath,
        //     const OperationFlags &flags = {}) const = 0;

        // virtual Directory get_directory(
        //     const fs::path &relpath,
        //     const OperationFlags &flags = {}) const = 0;

        // virtual Directory locate(
        //     const fs::path &relpath,
        //     const std::vector<std::string> &filename_masks,
        //     const core::types::TaggedValueList &attribute_filters = {},
        //     const OperationFlags &flags = {}) const = 0;

        // virtual void copy_from(
        //     const ContextProxy &other_context,
        //     const fs::path &other_path,
        //     const fs::path &this_path,
        //     const OperationFlags &flags = {}) const = 0;

        // virtual void move_from(
        //     const ContextProxy &other_context,
        //     const fs::path &other_path,
        //     const fs::path &this_path,
        //     const OperationFlags &flags = {}) const = 0;

        // virtual void remove(
        //     const fs::path &relpath,
        //     const OperationFlags &flags = {}) const = 0;

        // virtual void create_folder(
        //     const fs::path &relpath,
        //     const OperationFlags &flags) const = 0;

        // virtual UniqueReader read_file(
        //     const fs::path &relpath) const = 0;

        // virtual UniqueWriter write_file(
        //     const fs::path &relpath) const = 0;

        // virtual core::types::KeyValueMap get_attributes(
        //     const fs::path &relpath) const = 0;

        // virtual void set_attributes(
        //     const fs::path &relpath,
        //     const core::types::KeyValueMap &attributes) const = 0;

        // virtual void clear_attributes(
        //     const fs::path &relpath) const = 0;

    public:
        fs::path relpath;
    };

    using LocationList = std::vector<Location>;
}  // namespace platform::vfs

