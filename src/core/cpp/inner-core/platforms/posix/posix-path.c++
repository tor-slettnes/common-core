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

}  // namespace core::platform
