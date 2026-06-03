// -*- c++ -*-
//==============================================================================
/// @file netconfig-grpc-requesthandler.c++
/// @brief Handle NetConfig gRPC service requests
/// @author Tor Slettnes
//==============================================================================

#include "netconfig-grpc-requesthandler.h++"
#include "netconfig-grpc-signalqueue.h++"
#include "protobuf-netconfig-types.h++"
#include "protobuf-inline.h++"
#include "types/bytevector.h++"

namespace cc::platform::netconfig::grpc
{
    RequestHandler::RequestHandler()
        : provider(netconfig::network.get_shared())
    {
    }

    RequestHandler::RequestHandler(const std::shared_ptr<netconfig::ProviderInterface>& provider)
        : provider(provider)
    {
    }

    ::grpc::Status RequestHandler::GetHostName(
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::SetHostName(
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetConnections(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        platform::netconfig::protobuf::ConnectionMap* response)
    {
        try
        {
            cc::protobuf::encode(
                this->provider->get_connections(),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::DefineConnection(
        ::grpc::ServerContext* context,
        const platform::netconfig::protobuf::ConnectionRequest* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->define_connection(
                cc::protobuf::decoded<ConnectionData>(request->data()),
                request->activate());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::RemoveConnection(
        ::grpc::ServerContext* context,
        const platform::netconfig::protobuf::MappingKey* request,
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::ActivateConnection(
        ::grpc::ServerContext* context,
        const platform::netconfig::protobuf::MappingKey* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->activate_connection(request->key());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::DeactivateConnection(
        ::grpc::ServerContext* context,
        const platform::netconfig::protobuf::MappingKey* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->deactivate_connection(request->key());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetActiveConnections(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        platform::netconfig::protobuf::ActiveConnectionMap* response)
    {
        try
        {
            cc::protobuf::encode(
                this->provider->get_active_connections(),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::RequestScan(
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetAccessPoints(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        platform::netconfig::protobuf::AccessPointMap* response)
    {
        try
        {
            cc::protobuf::encode(
                this->provider->get_aps(),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::ConnectAccessPoint(
        ::grpc::ServerContext* context,
        const platform::netconfig::protobuf::WirelessConnectionRequest* request,
        ::google::protobuf::Empty* response)
    {
        ConnectionData data;
        std::string apname;
        cc::protobuf::decode(request->connection(), &data);

        try
        {
            switch (request->key_case())
            {
            case platform::netconfig::protobuf::WirelessConnectionRequest::KeyCase::kBssid:
                this->provider->connect_ap(request->bssid(), data);
                break;

            case platform::netconfig::protobuf::WirelessConnectionRequest::KeyCase::kSsid:
                this->provider->connect_ap(core::types::ByteVector(request->ssid()), data);
                break;

            default:
                break;
            }

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetDevices(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        platform::netconfig::protobuf::DeviceMap* response)
    {
        try
        {
            cc::protobuf::encode(
                this->provider->get_devices(),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetGlobalData(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        platform::netconfig::protobuf::GlobalData* response)
    {
        try
        {
            cc::protobuf::encode(
                *this->provider->get_global_data(),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::SetWirelessEnabled(
        ::grpc::ServerContext* context,
        const platform::netconfig::protobuf::RadioState* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->set_wireless_enabled(request->wireless_enabled());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::SetWirelessAllowed(
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::SelectWirelessBand(
        ::grpc::ServerContext* context,
        const platform::netconfig::protobuf::WirelessBandSetting* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->select_wireless_band(
                cc::protobuf::decoded<WirelessBandSelection>(request->band_selection()));

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::Watch(
        ::grpc::ServerContext* context,
        const cc::protobuf::signal::Filter* filter,
        ::grpc::ServerWriter<platform::netconfig::protobuf::Signal>* writer)
    {
        return this->stream_signals<platform::netconfig::protobuf::Signal, SignalQueue>(
            context,
            filter,
            writer);
    }
}  // namespace cc::platform::netconfig::grpc
