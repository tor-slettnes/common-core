// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-product.c++
/// @brief SysConfig gRPC implementation - product information
/// @author Tor Slettnes
//==============================================================================

#include "sysconfig-grpc-product.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"

namespace cc::platform::sysconfig::grpc
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
            platform::sysconfig::protobuf::Signal::kProductInfo,
            [&](const platform::sysconfig::protobuf::Signal &signal) {
                sysconfig::signal_productinfo.emit(
                    cc::protobuf::decoded<ProductInfo>(signal.product_info()));
            });
    }

    ProductInfo ProductProvider::get_product_info() const
    {
        return cc::protobuf::decoded<ProductInfo>(
            this->client->call_check(
                &Client::Stub::GetProductInfo));
    }

    void ProductProvider::set_serial_number(const std::string &serial)
    {
        this->client->call_check(
            &Client::Stub::SetSerialNumber,
            cc::protobuf::encoded<::google::protobuf::StringValue>(serial));
    }

    void ProductProvider::set_model_name(const std::string &model)
    {
        this->client->call_check(
            &Client::Stub::SetModelName,
            cc::protobuf::encoded<::google::protobuf::StringValue>(model));
    }

}  // namespace cc::platform::sysconfig::grpc
