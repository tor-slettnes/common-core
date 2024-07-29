// -*- c++ -*-
//==============================================================================
/// @file netconfig-grpc-requesthandler.c++
/// @brief Handle NetConfig gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "netconfig-grpc-requesthandler.h++"
#include "netconfig-grpc-signalqueue.h++"
#include "protobuf-netconfig-types.h++"
#include "protobuf-inline.h++"
#include "types/bytevector.h++"

namespace platform::netconfig::grpc
{
    RequestHandler::RequestHandler()
        : provider(platform::netconfig::network.get_shared())
    {
    }

    RequestHandler::RequestHandler(const std::shared_ptr<netconfig::ProviderInterface>& provider)
        : provider(provider)
    {
    }

    ::grpc::Status RequestHandler::get_hostname(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::google::protobuf::StringValue* response)
    {
        try
        {
            response->set_value(
                this->provider->get_hostname());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::set_hostname(
        ::grpc::ServerContext* context,
        const ::google::protobuf::StringValue* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->set_hostname(request->value());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_connections(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::netconfig::ConnectionMap* response)
    {
        try
        {
            protobuf::encode(
                this->provider->get_connections(),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::define_connection(
        ::grpc::ServerContext* context,
        const ::cc::platform::netconfig::ConnectionRequest* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->define_connection(
                protobuf::decoded<ConnectionData>(request->data()),
                request->activate());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::remove_connection(
        ::grpc::ServerContext* context,
        const ::cc::platform::netconfig::MappingKey* request,
        ::google::protobuf::BoolValue* response)
    {
        try
        {
            bool removed = this->provider->remove_connection(request->key());
            response->set_value(removed);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::activate_connection(
        ::grpc::ServerContext* context,
        const ::cc::platform::netconfig::MappingKey* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->activate_connection(request->key());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::deactivate_connection(
        ::grpc::ServerContext* context,
        const ::cc::platform::netconfig::MappingKey* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->deactivate_connection(request->key());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_active_connections(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::netconfig::ActiveConnectionMap* response)
    {
        try
        {
            protobuf::encode(
                this->provider->get_active_connections(),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::request_scan(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->request_scan();
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_aps(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::netconfig::AccessPointMap* response)
    {
        try
        {
            protobuf::encode(
                this->provider->get_aps(),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::connect_ap(
        ::grpc::ServerContext* context,
        const ::cc::platform::netconfig::AccessPointConnection* request,
        ::google::protobuf::Empty* response)
    {
        ConnectionData data;
        std::string apname;
        protobuf::decode(request->connection(), &data);

        try
        {
            switch (request->key_case())
            {
            case cc::platform::netconfig::AccessPointConnection::KeyCase::kBssid:
                this->provider->connect_ap(request->bssid(), data);
                break;

            case cc::platform::netconfig::AccessPointConnection::KeyCase::kSsid:
                this->provider->connect_ap(core::types::ByteVector(request->ssid()), data);
                break;

            default:
                break;
            }

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_devices(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::netconfig::DeviceMap* response)
    {
        try
        {
            protobuf::encode(
                this->provider->get_devices(),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_global_data(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::netconfig::GlobalData* response)
    {
        try
        {
            protobuf::encode(
                *this->provider->get_global_data(),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::set_wireless_enabled(
        ::grpc::ServerContext* context,
        const ::cc::platform::netconfig::RadioState* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->set_wireless_enabled(request->wireless_enabled());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::set_wireless_allowed(
        ::grpc::ServerContext* context,
        const ::google::protobuf::BoolValue* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->set_wireless_allowed(request->value());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::select_wireless_band(
        ::grpc::ServerContext* context,
        const ::cc::platform::netconfig::WirelessBandSetting* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->select_wireless_band(
                protobuf::decoded<WirelessBandSelection>(request->band_selection()));

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::watch(
        ::grpc::ServerContext* context,
        const ::cc::signal::Filter* filter,
        ::grpc::ServerWriter<::cc::platform::netconfig::Signal>* writer)
    {
        return this->stream_signals<cc::platform::netconfig::Signal, SignalQueue>(
            context,
            filter,
            writer);
    }
}  // namespace platform::netconfig::grpc
