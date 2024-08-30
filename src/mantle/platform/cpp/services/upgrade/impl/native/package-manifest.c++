// -*- c++ -*-
//==============================================================================
/// @file package-manifest.h++
/// @brief Manifest describing package contents
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-manifest.h++"

namespace platform::upgrade::native
{

    //==========================================================================
    // Product manifest

    std::string PackageManifest::product_name(const std::string &fallback) const
    {
        return this->get(SETTING_PRODUCT, fallback).as_string();
    }

    Version PackageManifest::version(const Version &fallback) const
    {
        core::types::Value version = this->get(SETTING_VERSION);
        if (auto *version_string = version.get_if<std::string>())
        {
            return Version::from_string(*version_string);
        }
        else if (core::types::ValueListRef parts = version.get_valuelist())
        {
            Version version;
            version.major = parts->get(0, 0).as_uint();
            version.minor = parts->get(1, 0).as_uint();
            version.patch = parts->get(2, 0).as_uint();
            version.build_number = parts->get(3, 0).as_uint();
            return version;
        }
        else
        {
            return fallback;
        }
    }

    std::string PackageManifest::description(const std::string &fallback) const
    {
        core::types::Value description = this->get(SETTING_DESCRIPTION);
        if (auto *description_text = description.get_if<std::string>())
        {
            return *description_text;
        }
        else if (core::types::ValueListRef parts = description.get_valuelist())
        {
            return core::str::join(parts->filter_by_type<std::string>(), "\n");
        }
        else
        {
            return fallback;
        }
    }

    bool PackageManifest::reboot_required(bool fallback) const
    {
        return this->get(SETTING_REBOOT, fallback).as_bool();
    }

    bool PackageManifest::product_match(const std::string &installed_product) const
    {
        if (const core::types::Value &product_match = this->get(SETTING_PRODUCT_MATCH))
        {
            std::regex rx(product_match.as_string());
            std::smatch match;

            if (std::regex_match(installed_product, match, rx))
            {
                return true;
            }
        }
        else if (!this->product_name().empty() && (this->product_name() == installed_product))
        {
            return true;
        }

        return false;
    }

    bool PackageManifest::version_match(const Version &installed_version) const
    {
        if (const core::types::Value &version_match = this->get(SETTING_VERSION_MATCH))
        {
            std::string installed_version_string = installed_version.to_string();
            std::regex rx(version_match.as_string());
            std::smatch match;

            if (std::regex_match(installed_version_string, match, rx))
            {
                return true;
            }
        }
        else if (this->version() > installed_version)
        {
            return true;
        }

        return false;
    }

}  // namespace platform::upgrade::native
