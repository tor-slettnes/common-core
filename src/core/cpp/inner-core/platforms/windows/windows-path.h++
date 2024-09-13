/// -*- c++ -*-
//==============================================================================
/// @file windows-path.h++
/// @brief Path-related functions - Windows specifics
/// @author Tor Slettnes <tor@slett.net>
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

        uint path_max_size() const noexcept override;
        std::string path_separator() const noexcept override;
        std::string dir_separator() const noexcept override;
        fs::path devnull() const noexcept override;
        fs::path tempfolder() const noexcept override;
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

    protected:
        fs::path fs_root() const;
    };

}  // namespace core::platform
