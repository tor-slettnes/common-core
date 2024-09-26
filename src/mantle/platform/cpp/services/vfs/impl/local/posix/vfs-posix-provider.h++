// -*- c++ -*-
//==============================================================================
/// @file vfs-posix-provider.h++
/// @brief VFS service - POSIX specific implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-local-provider.h++"
#include "types/valuemap.h++"

namespace platform::vfs::local
{
    class PosixProvider : public LocalProvider
    {
        using This = PosixProvider;
        using Super = LocalProvider;

    protected:
        PosixProvider(
            const std::string &name = "PosixProvider",
            core::platform::ProviderPriority priority = core::platform::PRIORITY_NORMAL);

    protected:
        bool filename_match(const std::vector<std::string> &masks,
                            const fs::path &basename,
                            bool include_hidden,
                            bool ignore_case) const override;

        FileInfo read_stats(const fs::path &localpath,
                            bool dereference = false) const override;

        fs::file_type path_type(mode_t mode) const;
    };

}  // namespace platform::vfs::local
