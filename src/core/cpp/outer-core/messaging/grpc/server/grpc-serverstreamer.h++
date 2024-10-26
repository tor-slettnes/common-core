/// -*- c++ -*-
//==============================================================================
/// @file grpc-serverstreamer.h++
/// @brief Server-side stream writer
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "thread/signaltemplate.h++"
#include "thread/blockingqueue.h++"
#include "status/exceptions.h++"
#include "platform/init.h++"

#include <grpcpp/impl/codegen/sync_stream.h>
#include <grpcpp/impl/codegen/server_context.h>

namespace core::grpc
{
    //==========================================================================
    /// @class ServerStreamer
    /// @brief Mix-in class to stream stream from queue to client

    template <class MessageT>
    class ServerStreamer : public types::BlockingQueue<MessageT>
    {
        using This = ServerStreamer<MessageT>;
        using Super = types::BlockingQueue<MessageT>;

    public:
        template <class... Args>
        ServerStreamer(Args &&...args)
            : Super(args...),
              shutdown_handle(core::platform::signal_shutdown.connect(
                  std::bind(&This::close, this)))
        {
        }

        virtual ~ServerStreamer()
        {
            core::platform::signal_shutdown.disconnect(this->shutdown_handle);
        }

        virtual void stream(::grpc::ServerContext *cxt,
                            ::grpc::ServerWriter<MessageT> *writer)
        {
            while (std::optional<MessageT> msg = this->get())
            {
                if (cxt->IsCancelled())
                {
                    break;
                }
                writer->Write(std::move(*msg));
            }
        }

    private:
        const std::string shutdown_handle;
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
