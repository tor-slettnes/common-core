/// -*- c++ -*-
//==============================================================================
/// @file posix-path.h++
/// @brief Path-related functions - POSIX specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/path.h++"

namespace core::platform
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
        fs::path default_config_folder() const noexcept override;
        fs::path default_data_folder() const noexcept override;
        fs::path default_log_folder() const noexcept override;

        FileStats readstats(const fs::path &path,
                            bool dereference) const noexcept override;
        fs::path readlink(const fs::path &path) const noexcept override;
        fs::path mktemp(const fs::path &folder,
                        const std::string &prefix = "tmp.",
                        const std::string &suffix = {}) override;
        fs::path mktempdir(const fs::path &folder,
                           const std::string &prefix,
                           const std::string &suffix) override;

    private:
        fs::file_type path_type(mode_t mode) const;

    };

}  // namespace core::platform
