// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-serverlistener.c++
/// @brief Stream log message from server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-grpc-serverlistener.h++"

#include "protobuf-logger-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-inline.h++"
#include "string/misc.h++"

namespace logger::grpc
{
    ServerListener::ServerListener(const std::shared_ptr<BaseListener>& provider)
        : provider(provider)
    {
    }

    ::grpc::Status ServerListener::listen(
        ::grpc::ServerContext* context,
        const ::cc::logger::ListenerSpec* request,
        ::grpc::ServerWriter<::cc::status::Event>* writer)
    {
        try
        {
            auto spec = protobuf::decoded<logger::ListenerSpec>(*request);
            if (spec.sink_id.empty())
            {
                spec.sink_id = core::str::url_decoded(context->peer());
            }

            auto queue = this->provider->listen(spec);

            while (const std::optional<core::status::Event::ptr>& event = queue->get())
            {
                if (context->IsCancelled())
                {
                    break;
                }

                writer->Write(protobuf::encoded_event(*event));
            }

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }
}  // namespace logger::grpc
