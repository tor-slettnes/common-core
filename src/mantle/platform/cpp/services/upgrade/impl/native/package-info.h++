// -*- c++ -*-
//==============================================================================
/// @file package-info.h++
/// @brief Information aobut package contents
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "upgrade-types.h++"
#include "platform/process.h++"
#include "types/value.h++"

namespace upgrade::native
{
    //==========================================================================
    // Package Information

    class NativePackageInfo : public PackageInfo
    {
        using This = NativePackageInfo;
        using Super = PackageInfo;

    public:
        using ptr = std::shared_ptr<NativePackageInfo>;

    public:
        NativePackageInfo(const fs::path &path,
                          const PackageSource &source,
                          const fs::path &staging_folder = {});

        NativePackageInfo(const core::types::KeyValueMap &settings,
                          const PackageSource &source,
                          const fs::path &staging_folder = {});

    public:
        bool is_applicable() const override;
        void check_applicable() const;
        fs::path staging_folder() const;

        core::platform::ArgVector install_command() const;
        core::platform::ArgVector finalize_command() const;

        std::string match_capture_total_progress() const;
        std::string match_capture_task_progress() const;
        std::string match_capture_task_description() const;


    private:
        static Version decode_version(const core::types::Value &value);
        static std::string decode_description(const core::types::Value &value);

        std::string capture_setting(const std::string &setting,
                                    const std::string &fallback = {}) const;

        bool is_applicable_product(const std::string &current_product) const;
        bool is_applicable_version(const Version &current_version) const;

    protected:
        core::types::KeyValueMap settings;

    private:
        fs::path staging_folder_;
        std::optional<bool> is_applicable_;
    };

}  // namespace upgrade::native
