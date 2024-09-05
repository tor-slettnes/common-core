// -*- c++ -*-
//==============================================================================
/// @file sysconfig-product.h++
/// @brief SysConfig service - Product Information API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-types.h++"
#include "platform/provider.h++"
#include "thread/signaltemplate.h++"

namespace platform::sysconfig
{
    //==========================================================================
    // Data types

    struct Version : public core::types::Streamable
    {
    public:
        static Version from_string(const std::string &version_string);
        static Version from_value(const core::types::Value &value);

        bool operator<(const Version &other) const;
        bool operator>(const Version &other) const;

    protected:
        void to_stream(std::ostream &stream) const override;
        void to_literal_stream(std::ostream &stream) const override;

    public:
        uint major = 0;
        uint minor = 0;
        uint patch = 0;
        uint build_number = 0;
        std::string printable_version;
    };

    using ComponentName = std::string;
    using ComponentVersions = std::map<ComponentName, Version>;

    struct ProductInfo : public core::types::Streamable
    {
        std::string product_name;
        std::string product_description;
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

namespace std
{
    // Sorting support for platform::sysconfig::Version
    template <>
    struct less<platform::sysconfig::Version>
    {
        inline bool operator()(const platform::sysconfig::Version &lhs,
                               const platform::sysconfig::Version &rhs) const
        {
            return lhs < rhs;
        }
    };
}  // namespace std
