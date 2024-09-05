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
    constexpr auto SETTING_PRODUCT = "product";
    constexpr auto SETTING_PRODUCT_MATCH = "product match";
    constexpr auto SETTING_VERSION = "version";
    constexpr auto SETTING_VERSION_MATCH = "version match";
    constexpr auto SETTING_DESCRIPTION = "description";
    constexpr auto SETTING_INSTALL_COMMAND = "install command";
    constexpr auto SETTING_REBOOT = "reboot required";
    constexpr auto DEFAULT_INSTALL_COMMAND = "install.sh";

    LocalManifest::LocalManifest(const core::SettingsStore &settings,
                                 const PackageSource &source)
        : Super(
              source,
              settings.get(SETTING_PRODUCT).as_string(),
              This::decode_version(settings.get(SETTING_VERSION).as_string()),
              This::decode_description(settings.get(SETTING_DESCRIPTION).as_string()),
              settings.get(SETTING_REBOOT, false).as_bool()),
          product_match(settings.get(SETTING_PRODUCT_MATCH).as_string()),
          version_match(settings.get(SETTING_VERSION_MATCH).as_string()),
          command(settings.get(SETTING_INSTALL_COMMAND, DEFAULT_INSTALL_COMMAND))
    {
    }

    const bool LocalManifest::is_applicable() const
    {
        try
        {
            this->check_applicable();
            return true;
        }
        catch (const std::exception &e)
        {
            logf_debug("Product %r version %s manifest not applicable: %s",
                       this->product(),
                       this->version(),
                       e);
            return false;
        }
    }

    void LocalManifest::check_applicable() const
    {
        platform::sysconfig::ProductInfo pi = platform::sysconfig::product->get_product_info();
        if (!this->is_applicable_product(pi.product_name))
        {
            throw core::exception::FailedPrecondition(
                "Package does not match installed product");
        }
        else if (!this->is_applicable_version(pi.release_version))
        {
            throw core::exception::FailedPrecondition(
                "Package version is not newer than installed version");
        }
    }

    core::platform::ArgVector LocalManifest::install_command() const
    {
        return core::platform::process->arg_vector(this->command);
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

    bool LocalManifest::is_applicable_product(const std::string &current_product) const
    {
        if (!this->product_match.empty())
        {
            std::regex rx(this->product_match);
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

    bool LocalManifest::is_applicable_version(const Version &current_version) const
    {
        if (!this->version_match.empty())
        {
            std::string current_version_string = current_version.to_string();
            std::regex rx(this->version_match);
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
