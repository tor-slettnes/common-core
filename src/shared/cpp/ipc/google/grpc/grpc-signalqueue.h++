/// -*- c++ -*-
//==============================================================================
/// @file grpc-signalqueue.h++
/// @brief A blocking queue to propagate signals from SignalServer<> to SignalClient<>
/// @author Tor Slettnes <tor@slett.net>
///
/// Class templates for Common Core gRPC services (client and server), including:
///  * Settings store in YourServiceName.json, using JsonCpp as backend
///  * Status/error code wrappers
//==============================================================================

#pragma once
#include "thread/blockingqueue.h++"
#include "thread/signaltemplate.h++"
#include "signal_types.pb.h"

#include <functional>
#include <unordered_set>

namespace cc::grpc
{
    //==========================================================================
    /// \class SignalQueue<SignalType>
    /// \brief Emit Switbboard signals to a single client over gRPC
    ///
    /// \tparam ServiceType
    ///     gRPC service type
    /// \tparam SignalMessageType
    ///     Signal class, derfined in .proto file
    ///
    /// \class SignalQueue
    ///
    /// Each SignalQueue instance represents an active gRPC stream writer, which
    /// in turn is created by the Server::watch() method, below.
    ///
    /// Implementations should overwrite the `initialize()` method to connect
    /// specific underlying Signal<T> or MappedSignal<T> instances to a
    /// corresponding handler method, which should then
    ///  * encode its payload of emitted signals to a corresponding protobuf
    ///    Signal() message and
    ///  * append the resulting protobuf message to this queue.
    /// (The `connect()` method emplate, below, can be used to help with this).
    /// The server's `watch()` method will then stream these messages back to
    /// the client from this queue.
    ///
    /// It is also important to override the `deinitialize()` method to
    /// explicitly disconnect from these same signals, or else the handlers
    /// remain connected to the signal after the client has disconnected and
    /// this SignalQueue() object has been deleted, which in turn may crash the
    /// server.
    ///
    /// For sample SignalQueue implementations, see:
    ///  * `sysconfig::SignalQueue()` or `upgrade::SignalQueue()`
    ///    for forwarding of basic Signal<T> events,
    ///  * `vfs::SignalQueue()` or `switchboard::SignalQueue()`
    ///    for forwarding of MappedSignal<T> events.

    template <class MessageType>
    class SignalQueue : public types::BlockingQueue<MessageType>
    {
    protected:
        using SignalFilter = CC::Signal::Filter;

        template <class T>
        using Encoder = std::function<void(T, MessageType *)>;

        template <class T, class K = std::string>
        using MappedEncoder = std::function<
            void(signal::MappingChange, K, T, MessageType *)>;

    public:
        SignalQueue(const SignalFilter &filter,
                    const std::string &id,
                    uint maxsize = 0)
            : types::BlockingQueue<MessageType>(maxsize),
              id(id),
              filter_polarity(filter.polarity()),
              filter_indices(filter.indices().begin(), filter.indices().end())
        {
        }

        /// Implementations should override this in order to connect specific
        /// Signal<T> or MappedSignal<T> instances to corresponding handlers,
        /// which in turn will encode the payload and add the result to this
        /// queue.
        virtual void initialize() {}

        /// Implementations should override this in order to disconnect any
        /// signal handlers that were connected in their `initialize()` method.
        virtual void deinitialize() {}

    protected:
        /// \fn connect
        /// \brief Connect a signal of type Signal<T> for encoding/enqueung
        /// \tparam SignalType
        ///    Signal data type
        /// \param[in] signal
        ///    Signal to which to connect
        /// \param[in] encoder
        ///    Routine to encode signal data to ProtoBuf Signal message

        template <class SignalType>
        void connect(uint signal_index,
                     signal::Signal<SignalType> &signal,
                     const Encoder<SignalType> &encoder)
        {
            if (this->is_included(signal_index))
            {
                signal.connect(
                    this->id,
                    [=](const SignalType &value) {
                        MessageType msg;
                        encoder(value, &msg);
                        this->put(std::move(msg));
                    });
            }
        }

        /// \fn connect
        /// \brief
        ///    Connect a signal of type MappedSignal<T> for encoding/enqueung
        ///    ProtoBuf messages with `change` and `key` fields.
        /// \tparam T
        ///    Signal data type
        /// \param[in] signal_index
        ///     Signal enumeration in .proto file, to decide whether to actually connect.
        /// \param[in] signal
        ///    Signal to which to connect
        /// \param[in] encoder
        ///    Routine to encode signal data to ProtoBuf Signal message

        template <class SignalType>
        void connect(uint signal_index,
                     signal::MappedSignal<SignalType> &signal,
                     const Encoder<SignalType> &encoder)
        {
            if (this->is_included(signal_index))
            {
                signal.connect(
                    this->id,
                    [=](signal::MappingChange change,
                        const std::string &key,
                        const SignalType &value) {
                        MessageType msg;
                        msg.set_change(static_cast<CC::Signal::MappingChange>(change));
                        msg.set_key(key);
                        encoder(value, &msg);
                        this->put(std::move(msg));
                    });
            }
        }

        /// \fn connect
        /// \brief Connect a signal of type MappedSignal<SignalType> for encoding/enqueung
        /// \tparam SignalType
        ///    Signal data type
        /// \param[in] signal_index
        ///     Signal enumeration in .proto file, to decide whether to actually connect.
        /// \param[in] signal
        ///    Signal to which to connect
        /// \param[in] encoder
        ///    Routine to encode signal data to ProtoBuf Signal message

        template <class SignalType>
        void connect(uint signal_index,
                     signal::MappedSignal<SignalType> &signal,
                     const MappedEncoder<SignalType> &encoder)
        {
            if (this->is_included(signal_index))
            {
                signal.connect(
                    this->id,
                    [=](signal::MappingChange change,
                        const std::string &key,
                        const SignalType &value) {
                        MessageType msg;
                        encoder(change, key, value, &msg);
                        this->put(std::move(msg));
                    });
            }
        }

        /// \fn disconnect
        /// \brief Disconnect from a signal
        /// \tparam SignalType
        ///    Signal data tyhpe
        /// \param[in] signal
        ///    Signal from which to disconnect

        template <class SignalType>
        void disconnect(SignalType &signal)
        {
            signal.disconnect(this->id);
        }

    private:
        bool is_included(uint signal_index)
        {
            bool filtered = this->filter_indices.count(signal_index);
            return (filtered == this->filter_polarity);
        }

    protected:
        std::string id;
        bool filter_polarity;
        std::unordered_set<uint> filter_indices;
    };
}  // namespace cc::grpc

using cc::grpc::SignalQueue;
