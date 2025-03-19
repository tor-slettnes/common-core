/// -*- c++ -*-
//==============================================================================
/// @file posix-path.c++
/// @brief Path-related functions - POSIX specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-path.h++"
#include "buildinfo.h++"

#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>  // readlink()
#include <limits.h>  // PATH_MAX
#include <fnmatch.h>
#include <pwd.h>
#include <grp.h>

#include <cstring>

namespace core::platform
{
    FileStats PosixPathProvider::get_stats(
        const fs::path &path,
        bool dereference) const
    {
        struct stat statbuf;
        std::memset(&statbuf, 0, sizeof(statbuf));
        // int (*statmethod)(const char *pathname, struct stat *statbuf);

        auto statmethod = dereference ? ::stat : ::lstat;
        if (statmethod(path.c_str(), &statbuf) == -1)
        {
            throw fs::filesystem_error(
                strerror(errno),
                path,
                std::error_code(errno, std::generic_category()));
        }

        return {
            .type = this->path_type(statbuf.st_mode),
            .size = static_cast<std::size_t>(statbuf.st_size),
            .link = this->readlink(path, statbuf),
            .mode = statbuf.st_mode,
            .readable = ::access(path.c_str(), R_OK) == 0,
            .writable = ::access(path.c_str(), W_OK) == 0,
            .uid = statbuf.st_uid,
            .gid = statbuf.st_gid,
            .owner = user->get_username(statbuf.st_uid),
            .group = user->get_groupname(statbuf.st_gid),
            .access_time = dt::to_timepoint(statbuf.st_atim),
            .modify_time = dt::to_timepoint(statbuf.st_mtim),
            .create_time = dt::to_timepoint(statbuf.st_ctim),
        };
    }

    bool PosixPathProvider::is_readable(
        const fs::path &path,
        bool real_uid) const
    {
        auto method = real_uid ? ::access : ::euidaccess;
        return method(path.c_str(), R_OK) == 0;
    }

    bool PosixPathProvider::is_writable(
        const fs::path &path,
        bool real_uid) const
    {
        auto method = real_uid ? ::access : ::euidaccess;
        return method(path.c_str(), W_OK) == 0;
    }

    uint PosixPathProvider::path_max_size() const noexcept
    {
        return PATH_MAX;
    }

    std::string PosixPathProvider::path_separator() const noexcept
    {
        return ":";
    }

    std::string PosixPathProvider::dir_separator() const noexcept
    {
        return "/";
    }

    fs::path PosixPathProvider::devnull() const noexcept
    {
        return "/dev/null";
    }

    fs::path PosixPathProvider::tempfolder() const noexcept
    {
        return "/tmp";
    }

    fs::path PosixPathProvider::runstate_folder() const noexcept
    {
        return fs::path("/var/run") / ORGANIZATION;
    }

    std::optional<fs::path> PosixPathProvider::user_config_folder() const noexcept
    {
        const char *homedir = std::getenv("HOME");
        if (homedir && *homedir)
        {
            auto config_folder = fs::path(homedir) / ".config";
            if (fs::exists(config_folder))
            {
                return config_folder / ORGANIZATION;
            }
        }
        return {};
    }

    fs::path PosixPathProvider::readlink(const fs::path &path) const noexcept
    {
        struct stat statbuf;
        if (lstat(path.c_str(), &statbuf) != -1)
        {
            return this->readlink(path, statbuf);
        }
        else
        {
            return {};
        }
    }

    fs::path PosixPathProvider::mktemp(const fs::path &folder,
                                       const std::string &prefix,
                                       const std::string &suffix)
    {
        fs::path path = folder / (prefix + "XXXXXX" + suffix);
        std::string name = path.string();
        int fd = mkstemps(name.data(), suffix.length());
        if (fd < 0)
        {
            throw fs::filesystem_error(
                strerror(errno),
                path,
                std::error_code(errno, std::system_category()));
        }
        close(fd);
        return name;
    }

    fs::path PosixPathProvider::mktempdir(const fs::path &folder,
                                          const std::string &prefix,
                                          const std::string &suffix)
    {
        fs::path path_template = folder / (prefix + "XXXXXX" + suffix);
        if (char *c_path = ::mkdtemp(const_cast<char *>(path_template.c_str())))
        {
            return c_path;
        }
        else
        {
            throw fs::filesystem_error(
                strerror(errno),
                path_template,
                std::error_code(errno, std::generic_category()));
        }
    }

    bool PosixPathProvider::filename_match(
        const fs::path &mask,
        const fs::path &filename,
        bool match_leading_period,
        bool ignore_case) const
    {
        int flags = (match_leading_period ? 0 : FNM_PERIOD) |
                    (ignore_case ? FNM_CASEFOLD : 0);

        switch (int status = ::fnmatch(mask.c_str(), filename.c_str(), flags))
        {
        case 0:
            return true;

        case FNM_NOMATCH:
            return false;

        default:
            throw fs::filesystem_error(
                strerror(status),
                filename,
                fs::path(mask),
                std::error_code(status, std::generic_category()));
        }
    }

    fs::file_type PosixPathProvider::path_type(mode_t mode) const
    {
        static const std::unordered_map<mode_t, fs::file_type> modemap = {
            {S_IFREG, fs::file_type::regular},
            {S_IFDIR, fs::file_type::directory},
            {S_IFLNK, fs::file_type::symlink},
            {S_IFCHR, fs::file_type::character},
            {S_IFBLK, fs::file_type::block},
            {S_IFIFO, fs::file_type::fifo},
            {S_IFSOCK, fs::file_type::socket}};

        try
        {
            return modemap.at(mode & S_IFMT);
        }
        catch (const std::out_of_range &e)
        {
            return fs::file_type::none;
        }
    }

    fs::path PosixPathProvider::readlink(const fs::path &path,
                                         const struct stat &statbuf) const noexcept
    {
        fs::path linktarget;
        if ((statbuf.st_mode & S_IFMT) == S_IFLNK)
        {
            ssize_t size = (statbuf.st_size != 0
                            ? statbuf.st_size + 1
                            : this->path_max_size());
            std::vector<char> buf(size);
            ssize_t nbytes = ::readlink(path.c_str(), buf.data(), size);
            linktarget.assign(buf.data(), buf.data() + nbytes);
        }
        return linktarget;
    }

}  // namespace core::platform
