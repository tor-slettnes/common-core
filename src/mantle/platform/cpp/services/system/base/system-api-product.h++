// -*- c++ -*-
//==============================================================================
/// @file system-api-product.h++
/// @brief System service - Product Information API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/provider.h++"
#include "system-types.h++"
#include "thread/signaltemplate.h++"

namespace platform::system
{
    //==========================================================================
    // HostConfig provider

    class ProductProvider : public core::platform::Provider
    {
        using Super = core::platform::Provider;

    protected:
        using Super::Super;

    public:
        virtual ProductInfo get_product_info() const;
        virtual void set_serial_number(const std::string &serial);
        virtual void set_model_name(const std::string &model);
    };

    //==========================================================================
    // Provider instance

    extern core::platform::ProviderProxy<ProductProvider> product;

    //==========================================================================
    // Signals
    extern core::signal::DataSignal<ProductInfo> signal_productinfo;
}  // namespace platform::system
