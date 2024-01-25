/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-requesthandler.c++
/// @brief RPC functionalty over ZeroMQ using ProtoBuf - request handler
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-protobuf-requesthandler.h++"
#include "zmq-protobuf-error.h++"
#include "protobuf-event-types.h++"

#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace cc::zmq
{
    ProtoBufRequestHandler::ProtoBufRequestHandler(const std::string &interface_name)
        : interface_name_(interface_name)
    {
    }

    std::string ProtoBufRequestHandler::interface_name() const
    {
        return this->interface_name_;
    }

    std::string ProtoBufRequestHandler::full_method_name(const std::string &basename) const
    {
        return this->interface_name() + "::" + basename;
    }

    void ProtoBufRequestHandler::deinitialize()
    {
        this->clear_handlers();
    }

    void ProtoBufRequestHandler::process_method_request(const CC::RR::Request &request,
                                                        CC::RR::Reply *reply)
    {
        reply->set_client_id(request.client_id());
        reply->set_request_id(request.request_id());

        std::shared_ptr<ProtoBufError> error;

        if (auto handler = this->handler_map.get(request.method_name()))
        {
            try
            {
                handler(request.param(), reply->mutable_param());
            }
            catch (...)
            {
                logf_info(
                    "ZMQ ProtoBuf interface %r encountered failure "
                    "handling method %r() request: %s",
                    this->interface_name(),
                    request.method_name(),
                    std::current_exception());

                error = std::make_shared<ProtoBufError>(
                    CC::RR::STATUS_FAILED,
                    *cc::exception::map_to_event(std::current_exception()));
            }
        }
        else
        {
            logf_info(
                "ZMQ ProtoBuf interface %r received request for non-existing method: %s()",
                request.interface_name(),
                request.method_name());

            std::vector<std::string> method_names;
            method_names.reserve(this->handler_map.size());
            for (const auto &[name, method]: this->handler_map)
            {
                method_names.push_back(name);
            }

            logf_info(
                "Available methods are: %s",
                method_names);

            error = std::make_shared<ProtoBufError>(
                CC::RR::STATUS_CANCELLED,
                exception::NotFound(
                    "Method not found",
                    this->full_method_name(request.method_name())));
        }

        if (error)
        {
            CC::RR::Status *status = reply->mutable_status();
            status->set_code(error->status_code());
            protobuf::encode(*error, status->mutable_details());
        }
    }

    void ProtoBufRequestHandler::clear_handlers()
    {
        this->handler_map.clear();
    }
}  // namespace cc::zmq
