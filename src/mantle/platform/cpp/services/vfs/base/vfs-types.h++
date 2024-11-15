// -*- c++ -*-
//==============================================================================
/// @file vfs-types.h++
/// @brief VFS service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logging/logging.h++"
#include "types/filesystem.h++"
#include "types/listable.h++"
#include "types/value.h++"
#include "platform/path.h++"
#include "platform/user.h++"
#include "chrono/date-time.h++"

#include <iostream>

namespace platform::vfs
{
    define_log_scope("vfs");

    //==========================================================================
    /// Operational flags for copy(), move(), remove(), create_folder()

    struct OperationFlags
    {
        bool force = false;
        bool dereference = false;
        bool merge = false;
        bool update = false;
        bool with_attributes = false;
        bool include_hidden = false;
        bool ignore_case = false;
        bool inside_target = false;
    };

    std::ostream &operator<<(std::ostream &stream, const OperationFlags &flags);

    //==========================================================================
    // File information

    using FileMode = std::uint32_t;
    struct FileInfo
    {
        fs::file_type type = fs::file_type::none;  // regular, directory, etc..
        std::size_t size = 0;                      // Size in bytes
        fs::path link;                             // Target for symbolic links
        FileMode mode = 0;                         // UNIX mode mask
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

    core::types::TaggedValueList &operator<<(
        core::types::TaggedValueList &tvlist,
        const FileInfo &fileinfo);

    std::ostream &operator<<(
        std::ostream &stream,
        const FileInfo &fileinfo);


    //==========================================================================
    // Volume information

    using VolumeInfo = fs::space_info;

    //==========================================================================
    // Directory listing

    using Directory = std::map<fs::path, FileInfo>;
    using DirectoryEntry = std::pair<fs::path, vfs::FileInfo>;
    using DirectoryList = std::vector<DirectoryEntry>;

    //==========================================================================
    // Virtual path

    using ContextName = std::string;

    class Path : public core::types::Listable
    {
    public:
        Path(const ContextName &context = {}, const fs::path &path = {});
        operator bool() const noexcept;
        Path &operator/=(const fs::path &relpath);
        Path &operator+=(const fs::path &relpath);
        bool operator==(const Path &other) const;

        Path remove_filename() const;
        fs::path filename() const;

        void to_literal_stream(std::ostream &stream) const override;
        void to_stream(std::ostream &stream) const override;
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;

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
    // I/O streams
    using UniqueReader = std::unique_ptr<std::istream>;
    using UniqueWriter = std::unique_ptr<std::ostream>;

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

namespace std::filesystem
{
    core::types::TaggedValueList &operator<<(
        core::types::TaggedValueList &tvlist,
        const space_info &volinfo);

    std::ostream &operator<<(
        std::ostream &stream,
        const space_info &volinfo);

}
