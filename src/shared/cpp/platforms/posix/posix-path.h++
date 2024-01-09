/// -*- c++ -*-
//==============================================================================
/// @file posix-path.h++
/// @brief Path-related functions - POSIX specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/path.h++"

namespace cc::platform
{
    class PosixPathProvider : public PathProvider
    {
        using This = PosixPathProvider;
        using Super = PathProvider;

    public:
        using Super::Super;

        uint path_max_size() const noexcept override;
        std::string path_separator() const noexcept override;
        std::string dir_separator() const noexcept override;
        fs::path devnull() const noexcept override;
        fs::path tempfolder() const noexcept override;
        fs::path readlink(const fs::path &path) const noexcept override;
        fs::path mktemp(const fs::path &folder,
                        const std::string &prefix = "tmp.",
                        const std::string &suffix = {}) override;
    };

}  // namespace cc::platform
