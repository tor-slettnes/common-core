// -*- c++ -*-
//==============================================================================
/// @file sysconfig-product.c++
/// @brief SysConfig service - Product Information API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-product.h++"

namespace platform::sysconfig
{
    //==========================================================================
    // Data types

    void Version::to_stream(std::ostream &stream) const
    {
        core::types::TaggedValueList parts = {
            {"major", this->major},
            {"minor", this->minor},
            {"tweak", this->tweak},
        };

        parts.push_if(!this->printable_version.empty(), this->printable_version);
        parts.to_stream(stream);
    }


    void ProductInfo::to_stream(std::ostream &stream) const
    {
        core::types::PartsList parts;
        parts.add_string("product_model", this->product_model);
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
