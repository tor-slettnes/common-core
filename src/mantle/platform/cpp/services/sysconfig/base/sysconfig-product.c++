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
        const std::regex rx{"(\\d+)(?:\\.(\\d+))?(?:.(\\d+))(?:-(\\d+))"};

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

    void Version::to_stream(std::ostream &stream) const
    {
        core::types::PartsList parts;
        parts.add("major", this->major);
        parts.add("minor", this->minor);
        parts.add("patch", this->patch);

        parts.add_string("version_string",
                         this->printable_version,
                         !this->printable_version.empty());

        parts.add("build_number",
                  this->build_number,
                  this->build_number != 0);

        stream << parts;
    }

    void ProductInfo::to_stream(std::ostream &stream) const
    {
        core::types::PartsList parts;
        parts.add_string("product_name", this->product_name);
        parts.add_string("product_serial", this->product_serial);
        parts.add_string("hardware_model", this->hardware_model);
        parts.add("release_version", this->release_version, true, "%s");
        parts.add("component_versions", this->component_versions, true, "%s");
        parts.add("subsystem_info", this->subsystem_info, true, "%s");
        stream << parts;
    }

    //==========================================================================
    // Provider instance

    core::platform::ProviderProxy<ProductInterface> product("ProductInfo");

    //==========================================================================
    // Signals
    core::signal::DataSignal<ProductInfo> signal_productinfo("signal_productinfo", true);
}  // namespace platform::sysconfig
