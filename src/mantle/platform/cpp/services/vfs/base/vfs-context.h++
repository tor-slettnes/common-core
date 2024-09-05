// -*- c++ -*-
//==============================================================================
/// @file vfs-context.h++
/// @brief VFS service - abstract file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-types.h++"
#include "platform/path.h++"
#include "platform/user.h++"
#include "chrono/date-time.h++"
#include "types/value.h++"

#include <map>
#include <unordered_map>
#include <memory>
#include <iostream>

namespace platform::vfs
{
    using ContextName = std::string;

    //==========================================================================
    // File information

    struct FileStats
    {
        fs::file_type type = fs::file_type::none;  // regular, directory, etc..
        std::size_t size = 0;                      // Size in bytes
        fs::path link;                             // Target for symbolic links
        core::platform::FileMode mode = 0;         // UNIX mode mask
        bool readable = false;                     // Readable file/listable directory
        bool writable = false;                     // Writable file/modifiable directory
        core::platform::UID uid = 0;               // Owner numeric ID
        core::platform::GID gid = 0;               // Group numeric ID
        std::string owner;                         // Owner name
        std::string group;                         // Group name
        core::dt::TimePoint accessTime;            // Last access
        core::dt::TimePoint modifyTime;            // Last modification
        core::dt::TimePoint createTime;            // Creation
        core::types::KeyValueMap attributes;       // Custom file attributes
    };

    std::ostream &operator<<(std::ostream &stream, const FileStats &stats);

    //==========================================================================
    // Directory listing

    using Directory = std::map<fs::path, FileStats>;
    using DirectoryEntry = std::pair<fs::path, vfs::FileStats>;
    using DirectoryList = std::vector<DirectoryEntry>;

    //==========================================================================
    // Volume information

    using VolumeStats = fs::space_info;
    std::ostream &operator<<(std::ostream &stream, const VolumeStats &stats);

    //==========================================================================
    // Virtual path

    class Path : public core::types::Streamable
    {
    public:
        Path(const ContextName &context = {}, const fs::path &path = {});
        operator bool() const noexcept;
        Path &operator/=(const fs::path &relpath);
        Path &operator+=(const fs::path &relpath);
        bool operator==(const Path &other) const;

        void to_literal_stream(std::ostream &stream) const override;
        void to_stream(std::ostream &stream) const override;
        void check_relative() const;
        static void check_relative(const ContextName &context,
                                   const fs::path &relpath,
                                   const fs::path &abspath = {});

    public:
        ContextName context;
        fs::path relpath;
    };

    using Paths = std::vector<Path>;

    Path operator/(const Path &lhs, const fs::path &rhs);

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

        void to_stream(std::ostream &stream) const override;

    private:
        void add_ref();
        void del_ref();
        void check_access(bool modify) const;

    public:
        Context::ptr context;
        bool modify;
    };

    //==========================================================================
    /// \class Location
    /// \brief Reference to a specifc file/location within a VFS context.

    class Location : public ContextProxy
    {
    public:
        Location();

        Location(Context::ptr context,
                 const fs::path &relpath,
                 bool modify);

        Location(Context::ptr context,
                 bool modify);

        Location(const Location &other);

        fs::path localPath() const;
        fs::path localPath(const fs::path &relpath) const;
        Path virtualPath() const;
        Path virtualPath(const fs::path &relpath) const;

        void to_stream(std::ostream &stream) const override;

    public:
        fs::path relpath;
    };

    using LocationList = std::vector<Location>;
}  // namespace platform::vfs

namespace std
{
    /// Hashing support for vfs::Path, to allow instances to be used in unordered
    /// associative containers (like `std::unordered_map<vfs::Path, ...>`).
    template <>
    struct hash<platform::vfs::Path>
    {
        inline std::size_t operator()(const platform::vfs::Path &vpath) const
        {
            return std::hash<std::string>()(vpath.context) ^
                   std::hash<std::string>()(vpath.relpath);
        }
    };

    /// Sorting support for platform::vfs::Path, to allow instances to be used in ordered
    /// associative containers (like `std::map<platform::vfs::Path, ...>`).
    template <>
    struct less<platform::vfs::Path>
    {
        inline bool operator()(const platform::vfs::Path &lhs,
                               const platform::vfs::Path &rhs) const
        {
            if (lhs.context != rhs.context)
            {
                return std::less<std::string>()(lhs.context, rhs.context);
            }
            else
            {
                return std::less<std::string>()(lhs.relpath, rhs.relpath);
            }
        }
    };
}  // namespace std
