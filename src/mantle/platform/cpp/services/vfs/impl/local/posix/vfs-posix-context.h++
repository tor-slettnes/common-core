// -*- c++ -*-
//==============================================================================
/// @file vfs-posix-context.h++
/// @brief VFS service - local system context for POSIX systems
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-local-context.h++"

namespace platform::vfs::local
{
    //==========================================================================
    // PosixLocation

    class PosixLocation : public LocalLocation
    {
        using This = PosixLocation;
        using Super = LocalLocation;

    public:
        using Super::Super;

        FileInfo read_stats(
            const fs::path &localpath,
            bool dereference = false) const override;

        fs::file_type path_type(mode_t mode) const override;
    };
}  // namespace platform::vfs::local
