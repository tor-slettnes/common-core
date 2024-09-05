// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-product.h++
/// @brief SysConfig gRPC implementation - product information
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-product.h++"
#include "sysconfig-grpc-client.h++"

namespace platform::sysconfig::grpc
{
    class ProductProvider : public ProductInterface
    {
        using This = ProductProvider;
        using Super = ProductInterface;

    public:
        ProductProvider(const std::shared_ptr<Client> &client);

        void initialize() override;

        ProductInfo get_product_info() const override;
        void set_serial_number(const std::string &serial) override;
        void set_model_name(const std::string &model) override;

    private:
        std::shared_ptr<Client> client;
    };
}  // namespace platform::sysconfig::grpc
