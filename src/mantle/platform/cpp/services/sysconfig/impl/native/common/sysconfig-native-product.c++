// -*- c++ -*-
//==============================================================================
/// @file sysconfig-native-product.c++
/// @brief SysConfig native implementation - Product information
/// @author Tor Slettnes
//==============================================================================

#include "sysconfig-native-product.h++"
#include "platform/symbols.h++"

namespace sysconfig::native
{
    constexpr auto RELEASE_SETTINGS_FILE = "release.json";
    constexpr auto RELEASE_SETTING_PRODUCT = "product";
    constexpr auto RELEASE_SETTING_DESCRIPTION = "description";
    constexpr auto RELEASE_SETTING_VERSION = "version";

    constexpr auto PRODUCT_SETTINGS_FILE = "product.json";
    constexpr auto PRODUCT_SETTING_SERIAL = "serial number";
    constexpr auto PRODUCT_SETTING_MODEL = "hardware model";

    ProductProvider::ProductProvider()
        : Super(TYPE_NAME_FULL(This)),
          release_settings(core::SettingsStore::create_shared(RELEASE_SETTINGS_FILE)),
          product_settings(core::SettingsStore::create_shared(PRODUCT_SETTINGS_FILE))
    {
    }

    void ProductProvider::initialize()
    {
        Super::initialize();
        this->emit();
    }

    ProductInfo ProductProvider::get_product_info() const
    {
        ProductInfo pi;
        this->release_settings->reload();
        pi.product_name = this->get_product_name();
        pi.product_description = this->get_product_description();
        pi.serial_number = this->get_serial_number();
        pi.model_name = this->get_model_name();
        pi.release_version = this->get_release_version();
        pi.component_versions = this->get_component_versions();
        pi.subsystem_info = this->get_subsystem_info();
        return pi;
    }

    void ProductProvider::set_serial_number(const std::string &serial)
    {
        this->product_settings->insert_or_assign(PRODUCT_SETTING_SERIAL, serial);
        this->product_settings->save();
        this->emit();
    }

    void ProductProvider::set_model_name(const std::string &model)
    {
        this->product_settings->insert_or_assign(PRODUCT_SETTING_MODEL, model);
        this->product_settings->save();
        this->emit();
    }

    std::string ProductProvider::get_product_name() const
    {
        return this->release_settings->get(RELEASE_SETTING_PRODUCT).as_string();
    }

    std::string ProductProvider::get_product_description() const
    {
        return this->release_settings->get(RELEASE_SETTING_DESCRIPTION).as_string();
    }

    Version ProductProvider::get_release_version() const
    {
        return Version::from_value(this->release_settings->get(RELEASE_SETTING_VERSION));
    }

    std::string ProductProvider::get_serial_number() const
    {
        return this->product_settings->get(PRODUCT_SETTING_SERIAL).as_string();
    }

    std::string ProductProvider::get_model_name() const
    {
        return this->product_settings->get(PRODUCT_SETTING_MODEL).as_string();
    }

    ComponentVersions ProductProvider::get_component_versions() const
    {
        return {};
    }

    std::vector<ProductInfo> ProductProvider::get_subsystem_info() const
    {
        return {};
    }

    void ProductProvider::emit() const
    {
        signal_productinfo.emit(this->get_product_info());
    }

}  // namespace sysconfig::native
