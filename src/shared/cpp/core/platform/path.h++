/// -*- c++ -*-
//==============================================================================
/// @file path.h++
/// @brief Path-related functions - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include "string/misc.h++"
#include "types/bytevector.h++"
#include "types/filesystem.h++"
#include "types/platform.h++"

#include <iostream>
#include <filesystem>
#include <set>

/// Default filesystem paths.
namespace shared::platform
{
    /// @brief Abstract provider for path-related functions
    class PathProvider : public Provider
    {
        using Super = Provider;

    protected:
        /// @brief Constructor
        /// @param[in] provider_name
        ///    Provider name
        /// @param[in] programpath
        ///    Executable path, normally obtained from argv[0].
        PathProvider(const std::string &provider_name,
                     const std::string &exec_name);

    public:
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

        /// @brief return OS-specific temp folder
        virtual fs::path default_config_folder() const noexcept;

        /// @brief return OS-specific temp folder
        virtual fs::path default_data_folder() const noexcept;

        /// @brief return OS-specific temp folder
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
        /// @param[in] folder
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
        ///     @sa exec_path() until we find "share/settings".
        fs::path install_folder() const noexcept;

        /// @brief
        ///     Return search path for settings/configuration files.
        ///
        /// Obtain absolute folder names in which to look for configuration files.
        /// This may be specified using the "CONFIGPATH" environment variable.  If
        /// that variable is unset, use `/local/config` and `share/settings` in that
        /// order; the latter is relative to the parent directory of the directory
        /// containing the program.  For instance, if the if the executable is in
        /// /usr/local/bin, the search path becomes
        /// `/local/config:/usr/local/share/settings`.
        types::PathList settings_paths() const noexcept;

        /// @brief
        ///     Obtain folder for host-specific configuration files.
        ///     Identical to `settings_path().front()`.
        fs::path config_folder() const noexcept;

        ///     Obtain folder for host-specific configuration files.
        fs::path data_folder() const noexcept;

        ///     Obtain folder for log files.
        fs::path log_folder() const noexcept;

        /// Obtain the target of a symbolic link
        virtual fs::path readlink(const fs::path &path) const noexcept = 0;

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
                                const std::string &prefix = "tmp.",
                                const std::string &suffix = {}) = 0;

    private:
        fs::path exec_name_;
        fs::path installfolder_;
    };

    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<PathProvider> path;
}  // namespace shared::platform
