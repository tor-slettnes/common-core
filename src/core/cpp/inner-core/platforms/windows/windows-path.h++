/// -*- c++ -*-
//==============================================================================
/// @file windows-path.h++
/// @brief Path-related functions - Windows specifics
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "platform/path.h++"

namespace core::platform
{
    class WindowsPathProvider : public PathProvider
    {
        using This = WindowsPathProvider;
        using Super = PathProvider;

    public:
        WindowsPathProvider(const std::string &programpath);

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
        fs::path default_config_folder() const noexcept override;
        fs::path default_data_folder() const noexcept override;
        fs::path default_log_folder() const noexcept override;
        fs::path exec_path() const noexcept override;
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

    protected:
        fs::path fs_root() const;
    };

} // namespace core::platform
