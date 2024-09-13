/// -*- c++ -*-
//==============================================================================
/// @file posix-path.c++
/// @brief Path-related functions - POSIX specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-path.h++"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>  // readlink()
#include <limits.h>  // PATH_MAX
#include <pwd.h>
#include <grp.h>

#include <cstring>

namespace core::platform
{
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


    fs::path PosixPathProvider::default_config_folder() const noexcept
    {
        return "/etc/cc";
    }

    fs::path PosixPathProvider::default_data_folder() const noexcept
    {
        return "/data";
    }

    fs::path PosixPathProvider::default_log_folder() const noexcept
    {
        return "/var/log/picarro";
    }

    FileStats PosixPathProvider::readstats(const fs::path &path,
                                           bool dereference) const noexcept
    {
        struct stat statbuf;
        std::memset(&statbuf, 0, sizeof(statbuf));
        int (*statmethod)(const char *pathname, struct stat *statbuf);

        statmethod = dereference ? ::stat : ::lstat;

        struct passwd *pwd = nullptr;
        struct group *grp = nullptr;
        if (statmethod(path.c_str(), &statbuf) != -1)
        {
            pwd = ::getpwuid(statbuf.st_uid);
            grp = ::getgrgid(statbuf.st_gid);
        }

        return {
            .type = this->path_type(statbuf.st_mode),
            .size = static_cast<std::size_t>(statbuf.st_size),
            .link = core::platform::path->readlink(path),
            .mode = statbuf.st_mode,
            .readable = ::access(path.c_str(), R_OK) == 0,
            .writable = ::access(path.c_str(), W_OK) == 0,
            .uid = statbuf.st_uid,
            .gid = statbuf.st_gid,
            .owner = pwd ? pwd->pw_name : "",
            .group = grp ? grp->gr_name : "",
            .accessTime = core::dt::to_timepoint(statbuf.st_atim),
            .modifyTime = core::dt::to_timepoint(statbuf.st_mtim),
            .createTime = core::dt::to_timepoint(statbuf.st_ctim),
        };
    }

    fs::path PosixPathProvider::readlink(const fs::path &path) const noexcept
    {
        fs::path linktarget;
        struct stat statbuf;
        if ((lstat(path.c_str(), &statbuf) != -1) &&
            ((statbuf.st_mode & S_IFMT) == S_IFLNK))
        {
            ssize_t bufsiz = (statbuf.st_size != 0 ? statbuf.st_size + 1 : this->path_max_size());

            std::vector<char> buf(bufsiz);
            ssize_t nbytes = ::readlink(path.c_str(), buf.data(), bufsiz);
            linktarget.assign(buf.data(), buf.data() + nbytes);
        }
        return linktarget;
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
        if (char *c_path = ::mkdtemp(const_cast<char*>(path_template.c_str())))
        {
            return c_path;
        }
        else
        {
            throw fs::filesystem_error(
                strerror(errno),
                path_template,
                std::error_code(errno, std::system_category()));
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
}  // namespace core::platform
