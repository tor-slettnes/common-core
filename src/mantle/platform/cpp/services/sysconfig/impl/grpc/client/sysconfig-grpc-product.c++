// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-product.c++
/// @brief SysConfig gRPC implementation - product information
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-grpc-product.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"

namespace sysconfig::grpc
{
    ProductProvider::ProductProvider(const std::shared_ptr<Client> &client)
        : Super(TYPE_NAME_FULL(This)),
          client(client)
    {
    }

    void ProductProvider::initialize()
    {
        Super::initialize();
        this->client->add_handler(
            ::cc::sysconfig::Signal::kProductInfo,
            [&](const ::cc::sysconfig::Signal &signal) {
                sysconfig::signal_productinfo.emit(
                    protobuf::decoded<ProductInfo>(signal.product_info()));
            });
    }

    ProductInfo ProductProvider::get_product_info() const
    {
        return protobuf::decoded<ProductInfo>(
            this->client->call_check(
                &Client::Stub::get_product_info));
    }

    void ProductProvider::set_serial_number(const std::string &serial)
    {
        this->client->call_check(
            &Client::Stub::set_serial_number,
            protobuf::encoded<::google::protobuf::StringValue>(serial));
    }

    void ProductProvider::set_model_name(const std::string &model)
    {
        this->client->call_check(
            &Client::Stub::set_model_name,
            protobuf::encoded<::google::protobuf::StringValue>(model));
    }

}  // namespace sysconfig::grpc
