// -*- c++ -*-
//==============================================================================
/// @file sysconfig-product.h++
/// @brief SysConfig service - Product Information API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/provider.h++"
#include "thread/signaltemplate.h++"

namespace platform::sysconfig
{
    //==========================================================================
    // Data types

    struct Version : public core::types::Streamable
    {
        uint major = 0;
        uint minor = 0;
        uint tweak = 0;
        std::string printable_version;

    public:
        void to_stream(std::ostream &stream) const override;
    };

    using ComponentName = std::string;
    using ComponentVersions = std::map<ComponentName, Version>;

    struct ProductInfo : public core::types::Streamable
    {
        std::string product_model;
        std::string product_serial;
        std::string hardware_model;

        Version release_version;
        ComponentVersions component_versions;
        std::vector<ProductInfo> subsystem_info;

    public:
        void to_stream(std::ostream &stream) const override;
    };

    //==========================================================================
    // Product provider interface

    class ProductInterface : public core::platform::Provider
    {
        using This = ProductInterface;
        using Super = core::platform::Provider;

    protected:
        using Super::Super;

    public:
        virtual ProductInfo get_product_info() const { return {}; }
        virtual void set_serial_number(const std::string &serial) {}
        virtual void set_model_name(const std::string &model) {}
    };

    //==========================================================================
    // Provider instance

    extern core::platform::ProviderProxy<ProductInterface> product;

    //==========================================================================
    // Signals
    extern core::signal::DataSignal<ProductInfo> signal_productinfo;
}  // namespace platform::sysconfig
