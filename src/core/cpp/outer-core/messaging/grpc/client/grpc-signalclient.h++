/// -*- c++ -*-
//==============================================================================
/// @file grpc-signalclient.h++
/// @brief Client-side wrapper functionality for Common Core gRPC services
/// @author Tor Slettnes <tor@slett.net>
///
/// Class templates for Common Core gRPC services (client and server), including:
///  * Settings store in YourServiceName.json, using JsonCpp as backend
///  * Status/error code wrappers
//==============================================================================

#pragma once
#include "grpc-clientwrapper.h++"
#include "grpc-clientreceiver.h++"
#include "signal.pb.h"
#include "protobuf-message.h++"
#include "protobuf-signal-receiver.h++"
#include "thread/binaryevent.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include <functional>

namespace core::grpc
{
    //==========================================================================
    /// @class SignalClient<ServiceT, SignalT>
    /// @brief Client wrapper with Signal<T> interface
    /// @tparam ServiceT
    ///     gRPC service type
    /// @tparam SignalT
    ///     Signal class, derfined in .proto file
    ///
    /// This template provides methods to stream back and handle Signal messages
    /// from services with corresponding streaming methods.  A typical .proto
    /// file is expected to contain something like this:
    ///
    /// @code{.proto}
    /// import "signal.proto";
    ///
    /// service MyService
    /// {
    ///     /// Watch for signals from th eserver
    ///     rpc watch (CC.Signal.Filter) returns (stream Signal);
    /// }
    ///
    /// message MySignal
    /// {
    ///     oneof signal {
    ///         DataType1 data1 = 8;
    ///         DataType2 data2 = 9;
    ///         ...
    ///     }
    /// }
    /// @endcode
    ///
    ///

    template <class ServiceT, class SignalT>
    class SignalClient : public ClientWrapper<ServiceT>,
                         public ::protobuf::SignalReceiver<SignalT>
    {
        using This = SignalClient<ServiceT, SignalT>;
        using Super = ClientWrapper<ServiceT>;
        using SignalReceiver = ::protobuf::SignalReceiver<SignalT>;

    protected:
        using SignalReader = std::unique_ptr<::grpc::ClientReader<SignalT>>;

    protected:
        template <class... Args>
        SignalClient(Args &&...args)
            : Super(std::forward<Args>(args)...),
              receiver(std::bind(&SignalReceiver::process_signal,
                                 this,
                                 std::placeholders::_1)),
              watching(false)
        {
        }

        virtual ~SignalClient()
        {
            this->deinitialize();
            this->stop_watching();
        }

    public:
        void initialize() override
        {
            SignalReceiver::initialize();
            ClientWrapper<ServiceT>::initialize();
        }

        void deinitialize() override
        {
            ClientWrapper<ServiceT>::deinitialize();
            SignalReceiver::deinitialize();
        }

        /// @brief Start watching for signals from server.
        ///
        /// @note
        ///     The server will immediately stream back any signals with the
        ///     `caching` option set to `true`. It is the callers responsibility
        ///     to ensure the corresponding Signal() messages are mapped to
        ///     corresponding signals on the client side before invoking this
        ///     method, so that any initial values are captured.  In other
        ///     words, make the appropriate `add_handler()` invocations prior
        ///     to `start_watching()`.
        ///
        /// @note
        ///     By default the server will connect to and stream back all of its
        ///     known signals.  Use \sa set_signal_filter() to apply a filter.

        inline virtual void start_watching()
        {
            // if (this->slots.empty())
            // {
            //     this->initialize();
            // }

            if (!this->watching)
            {
                this->watching = true;
                this->watch_start = steady::Clock::now();
                cc::signal::Filter filter = this->signal_filter();
                logf_debug("Invoking %s::watch(filter=%s)",
                           this->servicename(true),
                           filter);
                this->receiver.start(
                    &ServiceT::Stub::watch,
                    this->stub.get(),
                    filter);
            }
        }

        /// @brief Stop watching for signals from server
        inline void stop_watching()
        {
            if (this->watching)
            {
                logf_debug("Ending %s::watch()", this->servicename(true));
            }
            this->watching = false;
            this->completion_event.cancel();
            this->receiver.stop();
        }

        /// @brief
        ///    Indicate whether we are currently streaming signals from the server.
        /// @return
        ///    `true` if we are currently streaming signals, `false` otherwise.
        virtual inline bool watching_signals() const
        {
            return this->watching;
        }

        /// @brief
        ///    Block until all cached signals have been received from the server
        /// @param[in] deadline
        ///    Exit if the server cache has not been received within this time.
        /// @return
        ///    `true` if and only if the signal cache was received from the server
        inline bool wait_complete(const steady::TimePoint &deadline)
        {
            return this->completion_event.wait_until(deadline);
        }

        /// @brief
        ///    Block until all cached signals have been received from the server
        /// @param[in] timeout
        ///    Exit if the server cache has not been received within this duration
        ///    after the last `start_watching()` invocation.
        /// @return
        ///    `true` if and only if the signal cache was received from the server
        inline bool wait_complete(const dt::Duration &timeout)
        {
            return this->wait_complete(
                this->watch_start +
                std::chrono::duration_cast<steady::Duration>(timeout));
        }

    protected:
        inline void on_init_complete() override
        {
            logf_trace("Got completion, setting completion_event();");
            this->completion_event.set();
        }

    private:
        bool watching;

    protected:
        steady::TimePoint watch_start;
        std::thread watch_thread;
        std::shared_ptr<::grpc::ClientContext> watcher_context;
        ClientReceiver<ServiceT, SignalT, cc::signal::Filter> receiver;
        types::BinaryEvent completion_event;
    };

}  // namespace core::grpc
