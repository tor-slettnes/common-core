/// -*- c++ -*-
//==============================================================================
/// @file grpc-serverstreamer.h++
/// @brief Server-side stream writer
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "http-utils.h++"
#include "thread/signaltemplate.h++"
#include "thread/blockingqueue.h++"
#include "status/exceptions.h++"

#include <grpcpp/impl/codegen/sync_stream.h>
#include <grpcpp/impl/codegen/server_context.h>

namespace core::grpc
{
    //==========================================================================
    /// @class ServerStreamer
    /// @brief Stream messages from queue to client

    template <class MessageT>
    class ServerStreamer : public types::BlockingQueue<MessageT>
    {
        using This = ServerStreamer<MessageT>;
        using Super = types::BlockingQueue<MessageT>;

    public:
        using Super::Super;

        virtual void stream(::grpc::ServerContext *cxt,
                            ::grpc::ServerWriter<MessageT> *writer)
        {
            while (true)
            {
                std::optional<MessageT> msg = this->get();
                if (cxt->IsCancelled())
                {
                    break;
                }
                if (msg)
                {
                    writer->Write(*msg);
                }
            }
        }
    };

    //==========================================================================
    /// @class ServerSignalStreamer
    /// @brief Connect signal to write stream

    template <class MessageT>
    class ServerSignalStreamer : public ServerStreamer<MessageT>
    {
        using This = ServerSignalStreamer<MessageT>;
        using Super = ServerStreamer<MessageT>;

    public:
        ServerSignalStreamer(
            core::signal::DataSignal<MessageT> *signal,
            uint maxsize = 0,
            types::OverflowDisposition overflow = types::OverflowDisposition::DISCARD_OLDEST)
            : Super(maxsize, overflow),
              signal(signal)
        {
        }

        void stream(::grpc::ServerContext *cxt,
                    ::grpc::ServerWriter<MessageT> *writer) override
        {
            std::exception_ptr eptr;
            std::string handle = signal->connect([=](const MessageT &msg) {
                this->put(msg);
            });

            try
            {
                Super::stream(cxt, writer);
            }
            catch (...)
            {
                eptr = std::current_exception();
            }

            signal->disconnect(handle);

            if (eptr)
            {
                std::rethrow_exception(eptr);
            }
        }

    private:
        core::signal::DataSignal<MessageT> *signal;
    };

}  // namespace core::grpc
