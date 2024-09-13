/// -*- c++ -*-
//==============================================================================
/// @file posix-user.h++
/// @brief User related functions on POSIX compatible systems
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/user.h++"

namespace core::platform
{
    class PosixUserProvider : public UserProvider
    {
        using This = PosixUserProvider;
        using Super = UserProvider;

    protected:
        PosixUserProvider(const std::string &provider_name = "PosixUserProvider");

    public:
        UserName get_username(UID uid) const override;
        GroupName get_groupname(GID gid) const override;
    };

}  // namespace core::platform
