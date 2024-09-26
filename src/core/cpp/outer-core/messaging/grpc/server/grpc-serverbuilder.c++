// -*- c++ -*-
//==============================================================================
/// @file grpc-serverbuilder.c++
/// @brief gRPC server builder with endpoint lookup
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "grpc-serverbuilder.h++"
#include "grpc-serverinterceptors.h++"
#include "platform/dns-sd.h++"

#include <grpc++/ext/proto_server_reflection_plugin.h>

namespace core::grpc
{
    ServerBuilder::ServerBuilder(
        const std::string &listen_address,
        const std::shared_ptr<::grpc::ServerCredentials> &credentials,
        bool dnssd_advertise,
        bool enable_reflection)
        : ::grpc::ServerBuilder(),
          listen_address_(listen_address),
          credentials_(credentials),
          max_request_size_(0),
          max_reply_size_(0),
          dnssd_advertise_(dnssd_advertise)
    {
        if (enable_reflection)
        {
            ::grpc::reflection::InitProtoReflectionServerBuilderPlugin();
        }

        if (!listen_address.empty())
        {
            this->add_listener(listen_address);
        }
    }

    std::string ServerBuilder::listener_address() const
    {
        return this->listen_address_;
    }

    std::vector<std::string> ServerBuilder::listener_addresses() const
    {
        return {this->listeners_.begin(), this->listeners_.end()};
    }

    void ServerBuilder::add_handler_settings(std::shared_ptr<RequestHandlerBase> handler,
                                             bool add_listener)
    {
        if (add_listener)
        {
            this->add_listener(handler->address_setting());
        }

        this->adjust_max(handler->max_request_size(), &this->max_request_size_);
        this->adjust_max(handler->max_reply_size(), &this->max_reply_size_);

        if (this->dnssd_advertise_ &&
            core::platform::dns_sd &&
            !handler->dnssd_type().empty())
        {
            std::string host;
            uint port = 0;
            handler->splitaddress(
                add_listener ? handler->address_setting() : this->listener_address(),
                &host,
                &port);

            if (port != 0)
            {
                core::platform::dns_sd->advertise_service(
                    handler->servicename(true),  // name
                    handler->dnssd_type(),       // type
                    port,                        // port
                    {
                        // {"foo", "bar"},
                    });
            }
        }
    }

    void ServerBuilder::add_listener(const std::string &address)
    {
        if (!this->listeners_.count(address))
        {
            this->AddListeningPort(address, this->credentials_);
            this->listeners_.insert(address);
        }
    }

    void ServerBuilder::add_interceptors()
    {
        std::vector<std::unique_ptr<ServerInterceptorFactoryInterface>> creators;
        creators.push_back(LoggingInterceptorFactory::create_unique());
        // creators.push_back(EHInterceptorFactory::create_unique());
        this->experimental().SetInterceptorCreators(std::move(creators));
    }

    void ServerBuilder::adjust_max(uint candidate, uint *target)
    {
        if (candidate > *target)
        {
            *target = candidate;
        }
    }

    std::unique_ptr<::grpc::Server> ServerBuilder::BuildAndStart()
    {
        this->add_interceptors();

        if (this->max_request_size_)
        {
            this->SetMaxReceiveMessageSize(this->max_request_size_);
        }
        if (this->max_reply_size_)
        {
            this->SetMaxSendMessageSize(this->max_reply_size_);
        }

        std::unique_ptr<::grpc::Server> server = ::grpc::ServerBuilder::BuildAndStart();

        if (this->dnssd_advertise_ && core::platform::dns_sd)
        {
            core::platform::dns_sd->commit();
        }

        return server;
    }

}  // namespace core::grpc
