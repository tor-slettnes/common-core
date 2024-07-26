// -*- c++ -*-
//==============================================================================
/// @file network-grpc-requesthandler.c++
/// @brief Handle Network gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "network-grpc-requesthandler.h++"
#include "network-grpc-signalqueue.h++"
#include "protobuf-network.h++"
#include "protobuf-inline.h++"
#include "types/bytevector.h++"


namespace platform::network::grpc
{
    ::grpc::Status RequestHandler::get_hostname(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::google::protobuf::StringValue* response)
    {
        try
        {
            response->set_value(
                network->get_hostname());
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
            network->set_hostname(request->value());
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
        ::cc::network::ConnectionMap* response)
    {
        try
        {
            protobuf::encode(
                network->get_connections(),
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
        const ::cc::network::ConnectionRequest* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            network->define_connection(
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
        const ::cc::network::MappingKey* request,
        ::google::protobuf::BoolValue* response)
    {
        try
        {
            bool removed = network->remove_connection(request->key());
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
        const ::cc::network::MappingKey* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            network->activate_connection(request->key());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::deactivate_connection(
        ::grpc::ServerContext* context,
        const ::cc::network::MappingKey* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            network->deactivate_connection(request->key());
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
        ::cc::network::ActiveConnectionMap* response)
    {
        try
        {
            protobuf::encode(
                network->get_active_connections(),
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
            network->request_scan();
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
        ::cc::network::AccessPointMap* response)
    {
        try
        {
            protobuf::encode(
                network->get_aps(),
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
        const ::cc::network::AccessPointConnection* request,
        ::google::protobuf::Empty* response)
    {
        ConnectionData data;
        std::string apname;
        protobuf::decode(request->connection(), &data);

        try
        {
            switch (request->key_case())
            {
            case cc::network::AccessPointConnection::KeyCase::kBssid:
                network->connect_ap(request->bssid(), data);
                break;

            case cc::network::AccessPointConnection::KeyCase::kSsid:
                network->connect_ap(core::types::ByteVector(request->ssid()), data);
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
        ::cc::network::DeviceMap* response)
    {
        try
        {
            protobuf::encode(
                network->get_devices(),
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
        ::cc::network::GlobalData* response)
    {
        try
        {
            protobuf::encode(
                *network->get_global_data(),
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
        const ::cc::network::RadioState* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            network->set_wireless_enabled(request->wireless_enabled());
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
            network->set_wireless_allowed(request->value());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::select_wireless_band(
        ::grpc::ServerContext* context,
        const ::cc::network::WirelessBandSetting* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            network->select_wireless_band(
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
        ::grpc::ServerWriter<::cc::network::Signal>* writer)
    {
        return this->stream_signals<cc::network::Signal, SignalQueue>(
            context,
            filter,
            writer);
    }
}  // namespace platform::network::grpc
