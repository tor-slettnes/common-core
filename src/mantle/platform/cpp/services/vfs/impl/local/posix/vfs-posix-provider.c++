// -*- c++ -*-
//==============================================================================
/// @file vfs-posix-provider.c++
/// @brief VFS service - POSIX specific implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-posix-provider.h++"
#include "vfs.h++"
#include "status/exceptions.h++"
#include "platform/path.h++"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>  // statvfs(3)
#include <unistd.h>       // stat(2)
#include <pwd.h>          // getpwuid(3)
#include <grp.h>          // getgruid(3)
#include <fnmatch.h>      // fnmatch(3)

#include <cstring>  // std::memset()

namespace platform::vfs::local
{
    PosixProvider::PosixProvider(const std::string &name,
                                 core::platform::ProviderPriority priority)
        : Super(name, priority)
    {
    }

    bool PosixProvider::filename_match(
        const std::vector<std::string> &masks,
        const fs::path &basename,
        bool include_hidden,
        bool ignore_case) const
    {
        int flags = (include_hidden ? 0 : FNM_PERIOD) |
                    (ignore_case ? FNM_CASEFOLD : 0);

        for (const std::string &mask : masks)
        {
            if (fnmatch(mask.c_str(), basename.c_str(), flags) == 0)
            {
                return true;
            }
        }
        return false;
    }

    FileStats PosixProvider::read_stats(
        const fs::path &localpath,
        bool dereference) const
    {
        struct stat statbuf;
        std::memset(&statbuf, 0, sizeof(statbuf));
        int (*statmethod)(const char *pathname, struct stat *statbuf);

        statmethod = dereference ? ::stat : ::lstat;

        struct passwd *pwd = nullptr;
        struct group *grp = nullptr;
        if (statmethod(localpath.c_str(), &statbuf) != -1)
        {
            pwd = getpwuid(statbuf.st_uid);
            grp = getgrgid(statbuf.st_gid);
        }

        return {
            .type = this->path_type(statbuf.st_mode),
            .size = static_cast<std::size_t>(statbuf.st_size),
            .link = core::platform::path->readlink(localpath),
            .mode = statbuf.st_mode,
            .readable = ::access(localpath.c_str(), R_OK) == 0,
            .writable = ::access(localpath.c_str(), W_OK) == 0,
            .uid = statbuf.st_uid,
            .gid = statbuf.st_gid,
            .owner = pwd ? pwd->pw_name : "",
            .group = grp ? grp->gr_name : "",
            .accessTime = core::dt::to_timepoint(statbuf.st_atim),
            .modifyTime = core::dt::to_timepoint(statbuf.st_mtim),
            .createTime = core::dt::to_timepoint(statbuf.st_ctim),
        };
    }

    fs::file_type PosixProvider::path_type(mode_t mode) const
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

}  // namespace platform::vfs::local
