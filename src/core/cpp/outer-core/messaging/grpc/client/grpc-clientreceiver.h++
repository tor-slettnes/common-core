/// -*- c++ -*-
//==============================================================================
/// @file grpc-clientreceiver.h++
/// @brief Client-side stream reader
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "http-utils.h++"
#include "thread/signaltemplate.h++"
#include "status/exceptions.h++"

#include <grpcpp/impl/codegen/sync_stream.h>
#include <grpcpp/impl/codegen/client_context.h>

namespace core::grpc
{
    //==========================================================================
    /// @class ClientReceiver
    /// @brief Stream messages from server in background
    ///
    /// Subclasses should override `void process(const T &message)`
    /// to handle messages as they are received.

    template <class ServiceT, class MessageT, class RequestT = protobuf::Empty>
    class ClientReceiver
    {
        using This = ClientReceiver<ServiceT, MessageT, RequestT>;

    protected:
        using Handler = std::function<void(const MessageT &)>;
        using UniqueReader = std::unique_ptr<::grpc::ClientReader<MessageT>>;

        using StreamerMethod =
            UniqueReader (ServiceT::Stub::*)(::grpc::ClientContext *, const RequestT &req);

    public:
        ClientReceiver(const Handler &handler)
            : handler(handler),
              keepalive(true)
        {
        }

        bool streaming() const
        {
            return this->receive_thread.joinable();
        }

        void start(const StreamerMethod &method,
                   typename ServiceT::Stub *stub,
                   const RequestT &request = {})
        {
            this->keepalive = true;
            if (!this->receive_thread.joinable())
            {
                this->receive_thread = std::thread(
                    &ClientReceiver::keep_streaming, this, method, stub, request);
            }
        }

        void stop()
        {
            this->keepalive = false;
            if (auto cxt = this->cxt)
            {
                cxt->TryCancel();
            }

            if (this->receive_thread.joinable())
            {
                this->receive_thread.join();
            }
        }

    private:
        void keep_streaming(const StreamerMethod &method,
                            typename ServiceT::Stub *stub,
                            const RequestT &request = {})
        {
            while (this->keepalive)
            {
                auto &cxt = this->cxt = std::make_shared<::grpc::ClientContext>();
                cxt->set_wait_for_ready(true);
                try
                {
                    this->stream((stub->*method)(cxt.get(), request));
                }
                catch (...)
                {
                    logf_notice("Stream failed: %s (%s)",
                                std::current_exception(),
                                cxt->debug_error_string());
                }

                if (this->keepalive)
                {
                    std::this_thread::sleep_for(2s);
                    logf_notice("Reconnecting to grpc service %r at %s",
                                ServiceT::service_full_name(),
                                core::http::url_decode(cxt->peer()));
                }
                this->cxt.reset();
            }
        }

        void stream(const UniqueReader &reader)
        {
            MessageT msg;
            logf_info("Connected to gRPC service: %s",
                      ServiceT::service_full_name());

            while (reader->Read(&msg))
            {
                this->handler(msg);
            }

            reader->Finish();
        }

    private:
        Handler handler;
        bool keepalive;
        std::shared_ptr<::grpc::ClientContext> cxt;
        std::thread receive_thread;
    };

    //==========================================================================
    /// @class ClientSignalReceiver
    /// @brief Stream messages from server and emit them locally as signals

    template <class ServiceT, class SignalT, class RequestT = protobuf::Empty>
    class ClientSignalReceiver : public ClientReceiver<ServiceT, SignalT, RequestT>
    {
        using Super = ClientReceiver<ServiceT, SignalT, RequestT>;
        using Signal = core::signal::DataSignal<SignalT>;

    public:
        ClientSignalReceiver(Signal *signal)
            : Super(std::bind(&Signal::emit, signal, std::placeholders::_1))
        {
        }
    };

}  // namespace core::grpc
