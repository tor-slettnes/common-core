// -*- c++ -*-
//==============================================================================
/// @file sysconfig-product.c++
/// @brief SysConfig service - Product Information API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-product.h++"
#include "string/misc.h++"
#include "string/convert.h++"

namespace platform::sysconfig
{
    //==========================================================================
    // Data types

    Version Version::from_string(const std::string &version_string)
    {
        const std::regex rx{"(\\d+)(?:\\.(\\d+))?(?:\\.(\\d+))?(?:-(\\d+))?"};

        Version version;
        version.printable_version = version_string;

        std::smatch match;
        if (std::regex_match(version_string, match, rx))
        {
            version.major = core::str::convert_to<uint>(match.str(1), 0);
            version.minor = core::str::convert_to<uint>(match.str(2), 0);
            version.patch = core::str::convert_to<uint>(match.str(3), 0);
            version.build_number = core::str::convert_to<uint>(match.str(4), 0);
        }
        return version;
    }

    Version Version::from_value(const core::types::Value &value)
    {
        if (core::types::ValueListPtr list = value.get_valuelist())
        {
            Version version;
            version.major = list->get(0, 0);
            version.minor = list->get(1, 0);
            version.patch = list->get(2, 0);
            version.build_number = list->get(3, 0);
            return version;
        }
        else if (core::types::KeyValueMapPtr kvmap = value.get_kvmap())
        {
            Version version;
            version.major = kvmap->get("major", 0);
            version.minor = kvmap->get("minor", 0);
            version.patch = kvmap->get("patch", 0);
            version.build_number = kvmap->get("build number", 0);
            return version;
        }
        else if (const std::string *string = value.get_if<std::string>())
        {
            return Version::from_string(*string);
        }
        else
        {
            return {};
        }
    }

    Version::operator bool() const noexcept
    {
        return (this->major > 0) ||
               (this->minor > 0) ||
               (this->patch > 0) ||
               (this->build_number > 0);
    }

    bool Version::operator<(const Version &other) const
    {
        return (this->major != other.major)   ? (this->major < other.major)
               : (this->minor != other.minor) ? (this->minor < other.minor)
               : (this->patch != other.patch) ? (this->patch < other.patch)
                                              : (this->build_number < other.build_number);
    }

    bool Version::operator>(const Version &other) const
    {
        return other.operator<(*this);
    }

    void Version::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if(
            !this->printable_version.empty(),
            "printable",
            this->printable_version);

        tvlist->append(
            "canonical",
            core::str::format("%d.%d.%d", this->major, this->minor, this->patch));

        tvlist->append_if(
            this->build_number != 0,
            "build",
            this->build_number);
    }

    void Version::to_stream(std::ostream &stream) const
    {
        if (!this->printable_version.empty())
        {
            stream << this->printable_version;
        }
        else
        {
            core::str::format(stream,
                              "%d.%d.%d",
                              this->major,
                              this->minor,
                              this->patch);
        }
    }

    void Version::to_literal_stream(std::ostream &stream) const
    {
        this->as_tvlist().to_stream(stream);
    }

    void ProductInfo::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->extend({
            {"product_name", this->product_name},
            {"product_description", this->product_description},
            {"product_serial", this->product_serial},
            {"hardware_model", this->hardware_model},
            {"release_version", this->release_version.as_tvlist()},
        });

        if (!this->component_versions.empty())
        {
            core::types::TaggedValueList component_map;
            for (const auto &[component, version] : this->component_versions)
            {
                component_map.append(component, version.as_tvlist());
            }

            tvlist->append("component_versions", component_map);
        }

        if (!this->subsystem_info.empty())
        {
            core::types::ValueList subsystems;
            for (const ProductInfo &subsystem : this->subsystem_info)
            {
                subsystems.append(subsystem.as_tvlist());
            }

            tvlist->append("subsystem_info", subsystems);
        }
    }

    //==========================================================================
    // Provider instance

    core::platform::ProviderProxy<ProductInterface> product("ProductInfo");

    //==========================================================================
    // Signals
    core::signal::DataSignal<ProductInfo> signal_productinfo("signal_productinfo", true);
}  // namespace platform::sysconfig
