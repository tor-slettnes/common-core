/// -*- c++ -*-
//==============================================================================
/// @file path.h++
/// @brief Path-related functions - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include "chrono/date-time.h++"
#include "string/misc.h++"
#include "types/bytevector.h++"
#include "types/filesystem.h++"
#include "types/platform.h++"
#include "types/value.h++"
#include "user.h++"

#include <iostream>
#include <filesystem>
#include <set>

/// Default filesystem paths.
namespace core::platform
{
    //--------------------------------------------------------------------------
    // FileStats

    using FileMode = std::uint32_t;

    struct FileStats
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
        core::dt::TimePoint access_time;           // Last access
        core::dt::TimePoint modify_time;           // Last modification
        core::dt::TimePoint create_time;           // Creation
        core::types::KeyValueMap attributes;       // Custom file attributes
    };

    core::types::TaggedValueList &operator<<(
        core::types::TaggedValueList &tvlist,
        const FileStats &stats);

    std::ostream &operator<<(
        std::ostream &stream,
        const FileStats &stats);

    using DirectoryList = std::vector<fs::directory_entry>;

    //--------------------------------------------------------------------------
    /// @class PathProvider
    /// @brief Abstract provider for path-related functions

    class PathProvider : public Provider
    {
        using Super = Provider;

    protected:
        /// @brief Constructor
        /// @param[in] provider_name
        ///    Provider name
        /// @param[in] exec_name
        ///    Executable path, normally obtained from argv[0].
        PathProvider(const std::string &provider_name,
                     const std::string &exec_name);

    public:
        std::optional<FileStats> try_get_stats(const fs::path &path,
                                               bool dereference = false) const;

        virtual FileStats get_stats(const fs::path &path,
                                    bool dereference = false) const;

        virtual bool is_readable(const fs::path &path, bool real_uid = false) const = 0;
        virtual bool is_writable(const fs::path &path, bool real_uid = false) const = 0;

        /// @brief Return the maximum length of a filesystem path.
        virtual uint path_max_size() const noexcept = 0;

        /// @brief return the OS-specific path separator (':' on UNIX, ';' on Windows)
        virtual std::string path_separator() const noexcept = 0;

        /// @brief return the OS-specific directory separator ('/' on UNIX, '\\' on Windows)
        virtual std::string dir_separator() const noexcept = 0;

        /// @brief return OS-specific 'null' sink
        virtual fs::path devnull() const noexcept = 0;

        /// @brief return OS-specific temp folder
        virtual fs::path tempfolder() const noexcept = 0;

        /// @brief return OS-specific run state folder
        virtual fs::path runstate_folder() const noexcept = 0;

        /// @brief return user-specific settings folder
        virtual std::optional<fs::path> user_config_folder() const noexcept;

        /// @brief return OS-specific settings folder
        virtual fs::path default_config_folder() const noexcept;

        /// @brief return OS-specific data folder
        virtual fs::path default_data_folder() const noexcept;

        /// @brief return OS-specific log folder
        virtual fs::path default_log_folder() const noexcept;

        /// @brief Return the path to the running executable.
        virtual fs::path exec_path() const noexcept;

        /// @brief Return the path to the running executable folder.
        virtual fs::path exec_folder_path() const noexcept;

        /// @brief Return the base name of the running executable.
        virtual std::string exec_name(bool remove_extension = true,
                                      const std::string &fallback = "COMMAND") const noexcept;

        /// @brief
        ///     Walk up the diretory hierarchy from the specified folder to locate a
        ///     given name.
        /// @param[in] start
        ///     Path to the starting folder, which may be a file or directory
        /// @param[in] name
        ///     The name we are looking for
        /// @param[in] fallback
        ///     If we reached the root folder without finding file, return this.
        /// @return
        ///     The canonical path of the folder containing the specified name
        ///     (i.e., its parent).  If the name is not found, return fallback.

        fs::path locate_dominating_folder(const fs::path &start,
                                          const fs::path &name,
                                          const fs::path &fallback = "") const noexcept;

        /// @brief Return path with a default suffix added, if not already present.
        /// @param[in] pathname
        ///     Original path, with or without suffix
        /// @param[in] defaultSuffix
        ///     Suffix to add if not present
        /// @param[in] acceptAnySuffix
        ///     Whether to accept suffixes other than `defaultSuffix` in the
        ///     original path name.  If not, the returned path is guaranteed to end
        ///     with `defaultSuffix`.
        /// @return
        ///     Path name with suffix added.
        fs::path extended_filename(fs::path pathname,
                                   fs::path defaultSuffix,
                                   bool acceptAnySuffix = false) const noexcept;

        /// @brief
        ///     Determine the install folder for this application, using
        ///     @sa locateDominatingFolder() to look up the directory tree from
        ///     @sa exec_path() until we find "share/cc/settings".
        fs::path install_folder() const noexcept;

        /// @brief
        ///     Return search path for settings/configuration files.
        ///
        /// Obtain absolute folder names in which to look for configuration
        /// files.  This may be specified using the "CONFIGPATH" environment
        /// variable.  If that variable is unset, use `/etc/cc/settings` and
        /// followed by the paths returned by `default_settings_path()`, below.
        types::PathList settings_paths() const noexcept;

        /// @brief
        ///     Return search path for application-provided/default settings.
        ///
        /// Obtain absolute folder names in which to look for
        /// application-provided default. Typically this will be the single
        /// folder `share/cc/settings` within the parent directory of the
        /// directory containing the executable.  For example, if the executable
        /// is located inside `/usr/local/bin`, the search path beomes
        /// `{"/usr/local/share/cc/settings"}`.
        types::PathList default_settings_paths() const noexcept;

        /// @brief
        ///     Obtain folder for host-specific configuration files.
        ///     Identical to `settings_path().front()`.
        fs::path config_folder() const noexcept;

        ///     Obtain folder for host-specific configuration files.
        fs::path data_folder() const noexcept;

        ///     Obtain folder for log files.
        fs::path log_folder() const noexcept;

        /// Obtain the target of a symbolic link
        virtual fs::path readlink(const fs::path &path) const noexcept;

        /// @brief  Return the binary content of the specified file, up to the specified maximum.
        virtual types::ByteVector readdata(const fs::path &path,
                                           ssize_t maxsize = 0) const noexcept;

        /// @brief Return the contents off the specified file, up to the specified maximum
        virtual std::string readtext(const fs::path &path,
                                     const std::set<char> &striptrailing = WHITESPACE,
                                     ssize_t maxsize = 0,
                                     ssize_t bufsize = 2 << 12) const noexcept;

        /// @brief
        ///     Create a temporary file in the default temporary folder
        /// @param[in] prefix
        ///     Basename prefix
        /// @param[in] suffix
        ///     Basename suffix
        /// @note
        ///     Uniqueness and ownership is guaranteed via `mkstemps(3)`.
        ///     The file is created and immediately closed, allowing it to be
        ///     subsequently reopened by e.g. a subprocess.
        fs::path mktemp(const std::string &prefix = "tmp.",
                        const std::string &suffix = {});

        /// @brief
        ///     Create a temporary file in a specific folder
        /// @param[in] folder
        ///     Directory in which to create the file.
        /// @param[in] prefix
        ///     Basename prefix
        /// @param[in] suffix
        ///     Basename suffix
        /// @note
        ///     Uniqueness and ownership is guaranteed via `mkstemps(3)`.
        ///     The file is created and immediately closed, allowing it to be
        ///     subsequently reopened by e.g. a subprocess.
        virtual fs::path mktemp(const fs::path &folder,
                                const std::string &prefix,
                                const std::string &suffix) = 0;

        /// @brief
        ///     Create a temporary directory in a specific folder
        /// @param[in] prefix
        ///     Basename prefix
        /// @param[in] suffix
        ///     Basename suffix
        /// @note
        ///     Uniqueness and ownership is guaranteed via `mkdtemp(3)`.
        fs::path mktempdir(const std::string &prefix = "tmp.",
                           const std::string &suffix = {});

        /// @brief
        ///     Create a temporary directory in a specific folder
        /// @param[in] folder
        ///     Directory in which to create the file.
        /// @param[in] prefix
        ///     Basename prefix
        /// @param[in] suffix
        ///     Basename suffix
        /// @note
        ///     Uniqueness and ownership is guaranteed via `mkdtemp(3)`.
        virtual fs::path mktempdir(const fs::path &folder,
                                   const std::string &prefix,
                                   const std::string &suffix) = 0;

        /// @brief
        ///     Check if a file name matches a shell-style file name mask (globbing pattern)
        /// @param[in] masks
        ///     A shell-style filename mask (e.g. `*.txt`, `????-??-??.log`)
        /// @param[in] filename
        ///     Filename to check
        /// @param[in] match
        ///     Filename to check
        /// @param[in] match_leading_period
        ///     Whether wildcards (`*` and `?`) match a leading period in
        ///     a pathname component
        /// @param[in] ignore_case
        ///     Whether to use case-insensitive matching
        /// @return
        ///     `true` iff there were one or more hits.  `false` if no masks are provided.
        virtual bool filename_match(
            const fs::path &mask,
            const fs::path &filename,
            bool match_leading_period = false,
            bool ignore_case = false) const = 0;

        /// @brief
        ///     Check if a file name matches one or more shell-style masks (globbing patterns)
        /// @param[in] masks
        ///     Zero or more shell-style filename masks
        /// @param[in] filename
        ///     Filename to check
        /// @param[in] match_leading_period
        ///     Whether wildcards (`*` and `?`) match a leading period in
        ///     a pathname component
        /// @param[in] ignore_case
        ///     Whether to use case-insensitive matching
        /// @return
        ///     `true` iff there were one or more hits.  `false` if no masks are provided.
        /// @sa filename_match(std::string &, fs::path &)
        bool filename_match(
            const types::PathList &masks,
            const fs::path &filename,
            bool match_leading_period = false,
            bool ignore_case = false) const;

        /// @brief
        ///     Locate a relative path inside a folder
        /// @param[in] root
        ////    Parent folder inside which to search
        /// @param[in] filename_masks
        ///     Filename patterns for which to search
        /// @param[in] match_leading_period
        ///     Whether wildcards (`*` and `?`) match a leading period in
        ///     a pathname component
        /// @param[in] ignore_case
        ///     Whether to use case-insensitive matching

        std::vector<fs::directory_entry> locate(
            const types::PathList &filename_masks,
            const fs::path &root,
            bool match_leading_period = false,
            bool ignore_case = false) const;

    private:
        void locate_inside(
            const fs::path &root,
            const types::PathList &filename_masks,
            bool match_leading_period,
            bool ignore_case,
            std::vector<fs::directory_entry> *dir) const;

    protected:
    private:
        fs::path exec_name_;
        fs::path installfolder_;
    };

    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<PathProvider> path;
}  // namespace core::platform

namespace std::filesystem
{
    std::ostream &operator<<(std::ostream &stream, const file_type &type);
}
