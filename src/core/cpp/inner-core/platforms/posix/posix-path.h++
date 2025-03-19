/// -*- c++ -*-
//==============================================================================
/// @file posix-path.h++
/// @brief Path-related functions - POSIX specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/path.h++"

#include <sys/stat.h>
#include <unistd.h>

namespace core::platform
{
    class PosixPathProvider : public PathProvider
    {
        using This = PosixPathProvider;
        using Super = PathProvider;

    public:
        using Super::Super;

        FileStats get_stats(const fs::path &path,
                            bool dereference) const override;
        bool is_readable(const fs::path &path,
                         bool real_uid = false) const override;
        bool is_writable(const fs::path &path,
                         bool real_uid = false) const override;
        uint path_max_size() const noexcept override;
        std::string path_separator() const noexcept override;
        std::string dir_separator() const noexcept override;
        fs::path devnull() const noexcept override;
        fs::path tempfolder() const noexcept override;
        fs::path runstate_folder() const noexcept override;
        std::optional<fs::path> user_config_folder() const noexcept override;

        fs::path readlink(const fs::path &path) const noexcept override;
        fs::path mktemp(const fs::path &folder,
                        const std::string &prefix = "tmp.",
                        const std::string &suffix = {}) override;

        fs::path mktempdir(const fs::path &folder,
                           const std::string &prefix,
                           const std::string &suffix) override;

        bool filename_match(
            const fs::path &mask,
            const fs::path &filename,
            bool match_leading_period,
            bool ignore_case) const override;

    private:
        fs::path readlink(const fs::path &path,
                          const struct stat &statbuf) const noexcept;

    protected:
        fs::file_type path_type(mode_t mode) const;
    };

} // namespace core::platform
