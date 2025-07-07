/// -*- c++ -*-
//==============================================================================
/// @file grpc-signalrequesthandler.h++
/// @brief Common Core service provider with signal emitting capability
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "grpc-requesthandler.h++"
#include "grpc-signalqueue.h++"
#include "protobuf-message.h++"

#include "platform/symbols.h++"
#include "logging/logging.h++"

namespace core::grpc
{
    //==========================================================================
    /// @class SignalRequestHandler<ServiceT>
    /// @brief Service wrapper with signalling interface
    /// @tparam ServiceT
    ///     gRPC service type
    /// @tparam SignalT
    ///     Signal class, derfined in .proto file

    template <class ServiceT>
    class SignalRequestHandler : public RequestHandler<ServiceT>
    {
        using Super = RequestHandler<ServiceT>;

    protected:
        using Super::Super;

        // Serve requests to watch for signals
        template <class SignalT,
                  class SignalQueueT,
                  class SignalFilterT = cc::signal::Filter>
        inline ::grpc::Status stream_signals(::grpc::ServerContext *cxt,
                                             const SignalFilterT *req,
                                             ::grpc::ServerWriter<SignalT> *writer)
        {
            std::string who = this->servicename(true);
            try
            {
                logf_debug("Opening %s stream to client %s with filter %s",
                           SignalT().GetTypeName(),
                           cxt->peer(),
                           *req);
                SignalQueueT queue(platform::symbols->uuid(), *req);
                queue.initialize();
                // Append an empty message in the queue to signify the end of the cache.
                queue.put(SignalT());

                while (std::optional<SignalT> msg = queue.get())
                {
                    logf_trace("Sending signal to client %s: %s",
                               cxt->peer(),
                               msg.value());
                    writer->Write(msg.value());

                    if (cxt->IsCancelled())
                    {
                        break;
                    }
                }

                queue.deinitialize();

                logf_debug("Closing %s stream to client %s",
                           SignalT().GetTypeName(),
                           cxt->peer());
                return ::grpc::Status::OK;
            }
            catch (...)
            {
                return this->failure(std::current_exception(), *req, cxt->peer());
            }
        }
    };

}  // namespace core::grpc
