/// -*- c++ -*-
//==============================================================================
/// @file protobuf-signal-receiver.h++
/// @brief Handle ProtoBuf Signal messages
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "protobuf-message.h++"
#include "thread/signaltemplate.h++"
#include "signal.pb.h"  // Generated from `signal.proto`

#include <unordered_map>
#include <functional>

namespace protobuf
{

    template <class SignalT>
    class SignalReceiver
    {
        using This = SignalReceiver;

    protected:
        using Callback = std::function<void(const SignalT &signal)>;

        using MappingCallback = std::function<void(core::signal::MappingAction action,
                                                   const std::string &key,
                                                   const SignalT &signal)>;

        using SignalMap = std::unordered_map<typename SignalT::SignalCase, Callback>;

    protected:
        /// @brief
        ///     Override in derived classes to set up signal handlers.
        virtual void initialize()
        {
            this->register_handlers();
            this->add_handler(SignalT::SIGNAL_NOT_SET,
                              std::bind(&This::on_init_complete, this));
        }

        virtual void deinitialize()
        {
            this->unregister_handlers();
        }

        virtual void register_handlers()
        {
        }

        virtual void unregister_handlers()
        {
            this->slots.clear();
        }

    public:
        /// @brief Add a callback handler for a specific Signal type
        /// @param[in] signal_case
        ///     Enumerated index of a specific signal within the protobuf
        ///     `Signal` message, i.e. its number within the `oneof` clause.
        /// @param[in] callback
        ///     Method that will be invoked to handle corresponding Signal
        ///     messages as they are received from the server. Normally this
        ///     method will decode the corresponding field from within the
        ///     message and emit the result as a local Signal or MappingSignal
        ///     within the client process, thereby mirroring the original signal
        ///     that was emitted within the server.

        void add_handler(
            typename SignalT::SignalCase signal_case,
            const Callback &callback)
        {
            std::scoped_lock lck(this->slots_mtx);
            this->slots.emplace(signal_case, callback);
        }

        void add_mapping_handler(
            typename SignalT::SignalCase signal_case,
            const MappingCallback &callback)
        {
            std::scoped_lock lck(this->slots_mtx);
            this->slots.emplace(
                signal_case,
                [=](const SignalT &signal) {
                    callback(
                        static_cast<core::signal::MappingAction>(signal.mapping_action()),
                        signal.mapping_key(),
                        signal);
                });
        }

        /// @brief Add a callback handler for all Signal messages
        /// @param[in] callback
        ///     Method that will be invoked to handle Signal messages as they
        ///     are received from the server.  A typical use case will be to
        ///     re-emit the unmodified ProtoBuf message as a local signal,
        ///     which in turn may be connected to a publisher for a different
        ///     protocol/messaging system.
        void add_generic_handler(const Callback &callback)
        {
            this->geeneric_handler = callback;
        }

    protected:
        virtual void on_init_complete()
        {
        }

        cc::signal::Filter signal_filter()
        {
            std::scoped_lock lck(this->slots_mtx);
            cc::signal::Filter filter;
            filter.set_polarity(true);
            for (const auto &[index, callback] : this->slots)
            {
                filter.add_indices(index);
            }
            return filter;
        }

        static std::optional<bool> is_mapped(cc::signal::MappingAction action)
        {
            switch (action)
            {
            case cc::signal::MAP_ADDITION:
            case cc::signal::MAP_UPDATE:
                return true;

            case cc::signal::MAP_REMOVAL:
                return false;

            default:
                return {};
            }
        }

    public:
        void process_signal(const SignalT &msg)
        {
            std::scoped_lock lck(this->slots_mtx);

            // Invoke handler for this specific signal case, if any.
            this->process_signal_case(msg.signal_case(), msg);

            // Invoke generic Signal handler
            if (this->generic_handler)
            {
                (*this->generic_handler)(msg);
            }
        }

    protected:
        void process_signal_case(typename SignalT::SignalCase signal_case,
                                 const SignalT &msg)
        {
            auto it = this->slots.find(signal_case);
            if (it != this->slots.end())
            {
                (it->second)(msg);
            }
        }

    protected:
        SignalMap slots;
        std::optional<Callback> generic_handler;
        std::mutex slots_mtx;
    };

}  // namespace protobuf
