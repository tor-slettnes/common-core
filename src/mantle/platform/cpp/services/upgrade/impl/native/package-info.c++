// -*- c++ -*-
//==============================================================================
/// @file package-info.h++
/// @brief Information aobut package contents
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-info.h++"
#include "settings/settingsstore.h++"
#include "status/exceptions.h++"

namespace platform::upgrade::native
{
    constexpr auto SETTING_PRODUCT = "product";
    constexpr auto SETTING_PRODUCT_MATCH = "product match";
    constexpr auto SETTING_VERSION = "version";
    constexpr auto SETTING_VERSION_MATCH = "version match";
    constexpr auto SETTING_DESCRIPTION = "description";
    constexpr auto SETTING_INSTALL_COMMAND = "install command";
    constexpr auto SETTING_PROGRESS_CAPTURE = "progress capture";
    constexpr auto SETTING_CAPTURE_TOTAL_PROGRESS = "total progress";
    constexpr auto SETTING_CAPTURE_TASK_PROGRESS = "task progress";
    constexpr auto SETTING_CAPTURE_TASK_DESCRIPTION = "task description";

    constexpr auto SETTING_REBOOT = "reboot required";
    constexpr auto DEFAULT_INSTALL_COMMAND = "install.sh";

    NativePackageInfo::NativePackageInfo(const fs::path &settings_file,
                                 const PackageSource &source)
        : This(core::SettingsStore(settings_file), source)
    {
    }

    NativePackageInfo::NativePackageInfo(const core::types::KeyValueMap &settings,
                                 const PackageSource &source)
        : Super(
              source,
              settings.get(SETTING_PRODUCT).as_string(),
              This::decode_version(settings.get(SETTING_VERSION).as_string()),
              This::decode_description(settings.get(SETTING_DESCRIPTION).as_string()),
              settings.get(SETTING_REBOOT, false).as_bool()),
          settings(settings)
    {
    }

    bool NativePackageInfo::is_applicable() const
    {
        if (!this->is_applicable_.has_value())
        {
            try
            {
                this->check_applicable();
                logf_debug("Product %r version %s from %s is an applicable candidate",
                           this->product(),
                           this->version(),
                           this->source());
                const_cast<NativePackageInfo *>(this)->is_applicable_ = true;
            }
            catch (const std::exception &e)
            {
                logf_debug("Product %r version %s from %s is not applicable: %s",
                           this->product(),
                           this->version(),
                           this->source(),
                           e);
                const_cast<NativePackageInfo *>(this)->is_applicable_ = false;
            }
        }

        return this->is_applicable_.value();
    }

    void NativePackageInfo::check_applicable() const
    {
        platform::sysconfig::ProductInfo pi = platform::sysconfig::product->get_product_info();
        if (!this->is_applicable_product(pi.product_name))
        {
            throw core::exception::FailedPrecondition(
                "Package does not match installed product",
                {
                    {"provided", this->product()},
                    {"installed", pi.product_name},
                });
        }
        else if (!this->is_applicable_version(pi.release_version))
        {
            throw core::exception::FailedPrecondition(
                "Package version is not newer than installed version",
                {
                    {"provided", this->version()},
                    {"installed", pi.release_version},
                });
        }
    }

    core::platform::ArgVector NativePackageInfo::install_command() const
    {
        return core::platform::process->arg_vector(
            this->settings.get(SETTING_INSTALL_COMMAND,
                               DEFAULT_INSTALL_COMMAND));
    }

    std::string NativePackageInfo::match_capture_total_progress() const
    {
        return this->capture_setting(SETTING_CAPTURE_TOTAL_PROGRESS);
    }

    std::string NativePackageInfo::match_capture_task_progress() const
    {
        return this->capture_setting(SETTING_CAPTURE_TASK_PROGRESS);
    }

    std::string NativePackageInfo::match_capture_task_description() const
    {
        return this->capture_setting(SETTING_CAPTURE_TASK_DESCRIPTION);
    }

    std::string NativePackageInfo::capture_setting(const std::string &setting,
                                               const std::string &fallback) const
    {
        return this->settings
            .get(SETTING_PROGRESS_CAPTURE)
            .get(setting, fallback)
            .as_string();
    }

    Version NativePackageInfo::decode_version(const core::types::Value &value)
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

    std::string NativePackageInfo::decode_description(const core::types::Value &value)
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

    bool NativePackageInfo::is_applicable_product(const std::string &current_product) const
    {
        if (const core::types::Value &product_match = this->settings.get(SETTING_PRODUCT_MATCH))
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

    bool NativePackageInfo::is_applicable_version(const Version &current_version) const
    {
        if (const core::types::Value &version_match = this->settings.get(SETTING_VERSION_MATCH))
        {
            std::string current_version_string = current_version.to_string();
            std::regex rx(version_match.to_string());
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
