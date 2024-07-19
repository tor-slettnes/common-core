// -*- c++ -*-
//==============================================================================
/// @file system-provider-product.c++
/// @brief System service - Product Information API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-provider-product.h++"

namespace platform::system
{
    ProductInfo ProductProvider::get_product_info() const
    {
        return {};
    }

    void ProductProvider::set_serial_number(const std::string &serial)
    {
    }

    void ProductProvider::set_model_name(const std::string &model)
    {
    }

    //==========================================================================
    // Provider instance

    core::platform::ProviderProxy<ProductProvider> product("ProductInfo");

    //==========================================================================
    // Signals
    core::signal::DataSignal<ProductInfo> signal_productinfo("signal_productinfo", true);
}  // namespace platform::system
