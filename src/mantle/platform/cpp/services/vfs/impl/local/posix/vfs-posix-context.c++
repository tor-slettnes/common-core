// -*- c++ -*-
//==============================================================================
/// @file vfs-posix-context.c++
/// @brief VFS service - local system context for POSIX systems
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-local-context.h++"

#include <pwd.h>
#include <grp.h>
#include <unistd.h>

#include <cstring>

namespace platform::vfs::local
{
    //==========================================================================
    // PosixLocation

    FileStats PosixLocation::read_stats(
        const fs::path &localpath,
        bool dereference = false) const
    {
        struct stat statbuf;
        std::memset(&statbuf, 0, sizeof(statbuf));
        int (*statmethod)(const char *pathname, struct stat *statbuf);

        statmethod = dereference ? ::stat : ::lstat;

        struct passwd *pwd = nullptr;
        struct group *grp = nullptr;
        if (statmethod(localpath.c_str(), &statbuf) != -1)
        {
            pwd = ::getpwuid(statbuf.st_uid);
            grp = ::getgrgid(statbuf.st_gid);
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
