// -*- c++ -*-
//==============================================================================
/// @file sysconfig-product.h++
/// @brief SysConfig service - Product Information API
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "sysconfig-types.h++"
#include "platform/provider.h++"
#include "thread/signaltemplate.h++"
#include "types/listable.h++"

namespace sysconfig
{
    //--------------------------------------------------------------------------
    // Data types

    struct Version : public core::types::Listable
    {
    public:
        static Version from_string(const std::string &version_string);
        static Version from_value(const core::types::Value &value);

        operator bool() const noexcept;
        bool operator<(const Version &other) const;
        bool operator>(const Version &other) const;

    protected:
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;
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

    struct ProductInfo : public core::types::Listable
    {
        std::string product_name;
        std::string product_description;
        std::string serial_number;
        std::string model_name;

        Version release_version;
        ComponentVersions component_versions;
        std::vector<ProductInfo> subsystem_info;

    protected:
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;
    };

    //--------------------------------------------------------------------------
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

    //--------------------------------------------------------------------------
    // Provider instance

    extern core::platform::ProviderProxy<ProductInterface> product;

    //--------------------------------------------------------------------------
    // Signals

    extern core::signal::DataSignal<ProductInfo> signal_productinfo;
}  // namespace sysconfig

namespace std
{
    // Sorting support for sysconfig::Version
    template <>
    struct less<sysconfig::Version>
    {
        inline bool operator()(const sysconfig::Version &lhs,
                               const sysconfig::Version &rhs) const
        {
            return lhs < rhs;
        }
    };
}  // namespace std
