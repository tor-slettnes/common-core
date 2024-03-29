/// -*- c++ -*-
//==============================================================================
/// @file grpc-signalservice.h++
/// @brief Common Core service provider with signal emitting capability
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "grpc-requesthandler.h++"
#include "grpc-signalqueue.h++"
#include "protobuf-message.h++"

#include "platform/symbols.h++"

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

        // Serve requests to watch for signal changes
        template <class SignalT,
                  class SignalQueueT,
                  class SignalFilterT = CC::Signal::Filter>
        inline ::grpc::Status stream_signals(::grpc::ServerContext *cxt,
                                             const SignalFilterT *req,
                                             ::grpc::ServerWriter<SignalT> *writer)
        {
            try
            {
                logf_debug("Opening %s stream to client %s",
                           SignalT().GetTypeName(),
                           cxt->peer());
                SignalQueueT queue(platform::symbols->uuid(), *req);
                queue.initialize();

                while (true)
                {
                    std::optional<SignalT> msg = queue.get();
                    if (cxt->IsCancelled())
                    {
                        break;
                    }
                    if (msg)
                    {
                        logf_trace("Feeding signal to client %s: %s", cxt->peer(), *msg);
                        writer->Write(*msg);
                    }
                }

                queue.deinitialize();
                logf_debug("Closed %s stream to client %s",
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
