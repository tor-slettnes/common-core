// -*- c++ -*-
//==============================================================================
/// @file sysconfig-native-product.h++
/// @brief SysConfig native implementation - Product information
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-product.h++"
#include "settings/settingsstore.h++"

namespace sysconfig::native
{
    class ProductProvider : public ProductInterface
    {
        using This = ProductProvider;
        using Super = ProductInterface;

    public:
        ProductProvider();
        void initialize() override;

        ProductInfo get_product_info() const override;
        void set_serial_number(const std::string &serial) override;
        void set_model_name(const std::string &model) override;

    private:
        std::string get_product_name() const;
        std::string get_product_description() const;
        Version get_release_version() const;
        std::string get_serial_number() const;
        std::string get_model_name() const;
        ComponentVersions get_component_versions() const;
        std::vector<ProductInfo> get_subsystem_info() const;

        void emit() const;

    private:
        core::SettingsStore::ptr release_settings;
        core::SettingsStore::ptr product_settings;
    };
}  // namespace sysconfig::native
