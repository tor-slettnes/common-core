// -*- c++ -*-
//==============================================================================
/// @file package-manifest.h++
/// @brief Manifest describing package contents
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-manifest.h++"
#include "status/exceptions.h++"

namespace platform::upgrade::native
{
    LocalManifest::LocalManifest(const fs::path &manifest_file,
                                 const PackageSource &source)
        : SettingsStore(manifest_file),
          PackageManifest(
              source,
              this->get(SETTING_PRODUCT).as_string(),
              This::decode_version(this->get(SETTING_VERSION).as_string()),
              This::decode_description(this->get(SETTING_DESCRIPTION).as_string()),
              this->get(SETTING_REBOOT, false).as_bool())
    {
    }

    const bool LocalManifest::is_applicable() const
    {
        try
        {
            this->check_applicable();
            return true;
        }
        catch (const core::exception::FailedPrecondition &)
        {
            return false;
        }
    }

    void LocalManifest::check_applicable() const
    {
        platform::sysconfig::ProductInfo pi = platform::sysconfig::product->get_product_info();
        if (!this->product_match(pi.product_name))
        {
            throw core::exception::FailedPrecondition(
                "Package does not match installed product");
        }
        else if (!this->version_match(pi.release_version))
        {
            throw core::exception::FailedPrecondition(
                "Package version is not newer than installed version");
        }
    }

    Version LocalManifest::decode_version(const core::types::Value &value)
    {
        if (auto *version_string = value.get_if<std::string>())
        {
            return Version::from_string(*version_string);
        }
        else if (core::types::ValueListPtr parts = value.get_valuelist())
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
            return {};
        }
    }

    std::string LocalManifest::decode_description(const core::types::Value &value)
    {
        if (auto *description = value.get_if<std::string>())
        {
            return *description;
        }
        else if (core::types::ValueListPtr parts = value.get_valuelist())
        {
            return core::str::join(parts->filter_by_type<std::string>(), "\n");
        }
        else
        {
            return {};
        }
    }

    bool LocalManifest::product_match(const std::string &current_product) const
    {
        if (const core::types::Value &product_match = this->get(SETTING_PRODUCT_MATCH))
        {
            std::regex rx(product_match.as_string());
            std::smatch match;

            if (std::regex_match(current_product, match, rx))
            {
                return true;
            }
        }
        else if (!this->product().empty() && (this->product() == current_product))
        {
            return true;
        }

        return false;
    }

    bool LocalManifest::version_match(const Version &current_version) const
    {
        if (const core::types::Value &version_match = this->get(SETTING_VERSION_MATCH))
        {
            std::string current_version_string = current_version.to_string();
            std::regex rx(version_match.as_string());
            std::smatch match;

            if (std::regex_match(current_version_string, match, rx))
            {
                return true;
            }
        }
        else if (this->version() > current_version)
        {
            return true;
        }

        return false;
    }

}  // namespace platform::upgrade::native
