/// -*- c++ -*-
//==============================================================================
/// @file posix-user.c++
/// @brief User related functions on POSIX compatible systems
/// @author Tor Slettnes
//==============================================================================

#include "posix-user.h++"

#include <pwd.h>
#include <grp.h>

namespace core::platform
{
    PosixUserProvider::PosixUserProvider(const std::string &provider_name)
        : Super(provider_name)
    {
    }

    UserName PosixUserProvider::get_username(UID uid) const
    {
        struct passwd *pwd = ::getpwuid(uid);
        return pwd ? pwd->pw_name : "";
    }

    GroupName PosixUserProvider::get_groupname(GID gid) const
    {
        struct group *grp = ::getgrgid(gid);
        return grp ? grp->gr_name : "";
    }

}  // namespace core::platform
