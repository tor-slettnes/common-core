/// -*- c++ -*-
//==============================================================================
/// @file signalqueue.h++
/// @brief A blocking queue to propagate signals over DDS
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "thread/blockingqueue.h++"
#include "thread/signaltemplate.h++"

#include <functional>
#include <unordered_set>

namespace cc::signal
{

    //==========================================================================
    /// @class SignalQueue<SignalType>
    /// @brief Emit Switbboard signals to a single client over DDS
    ///
    /// @tparam MessageType
    ///     Signal structure, derfined in .idl file
    ///
    /// Implementations should overwrite the `initialize()` method to connect
    /// specific underlying Signal<T> or MappedSignal<T> instances to a
    /// corresponding handler method, which should then
    ///  * encode its payload of emitted signals to a corresponding protobuf
    ///    Signal() message and
    ///  * append the resulting protobuf message to this queue.
    /// (The `connect()` method template, below, can be used to help with this).
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
    class SignalQueue : public BlockingQueue<MessageType>
    {
    protected:
        struct SignalFilter
        {
            bool polarity = false;
            std::unordered_set<uint> indices;
        };

        template <class T>
        using Encoder = std::function<void(T, MessageType *)>;

        template <class T, class K = std::string>
        using MappedEncoder = std::function<void(MappingChange, K, T, MessageType *)>;

    public:
        SignalQueue(const SignalFilter &filter,
                    const std::string &id,
                    uint maxsize = 0)
            : BlockingQueue<MessageType>(maxsize),
              id(id),
              filter_polarity(filter.polarity()),
              filter_indices(filter.indices)
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
        /// @brief Connect a signal of type Signal<T> for encoding/enqueung
        /// @tparam T
        ///    Signal data type
        /// @param[in] signal
        ///    Signal to which to connect
        /// @param[in] encoder
        ///    Routine to encode signal data to ProtoBuf Signal message

        template <class T>
        void connect(Signal<T> &signal, const Encoder<T> &encoder)
        {
            signal.connect(
                this->id,
                [=](const T &value) {
                    MessageType msg;
                    encoder(value, &msg);
                    this->put(std::move(msg));
                });
        }

        /// @brief Connect a signal of type MappedSignal<T> for encoding/enqueung
        /// @tparam T
        ///    Signal data type
        /// @param[in] signal
        ///    Signal to which to connect
        /// @param[in] encoder
        ///    Routine to encode signal data to ProtoBuf Signal message

        template <class T>
        void connect(MappedSignal<T> &signal, const MappedEncoder<T> &encoder)
        {
            signal.connect(
                this->id,
                [=](MappingChange change, const std::string &key, const T &value) {
                    MessageType msg;
                    mapped_encoder(change, key, value, &msg);
                    this->put(std::move(msg));
                });
        }

        /// @brief Disconnect from a signal
        /// @tparam SignalType
        ///    Signal data tyhpe
        /// @param[in] signal
        ///    Signal from which to disconnect

        template <class SignalType>
        void disconnect(SignalType &signal)
        {
            signal.disconnect(this->id);
        }

    protected:
        std::string id;
        bool filter_polarity;
    };
}  // namespace cc::signal
