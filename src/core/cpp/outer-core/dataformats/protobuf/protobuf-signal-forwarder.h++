/// -*- c++ -*-
//==============================================================================
/// @file protobuf-signal-forwarder.h++
/// @brief Capture server-side signals and encode as ProtoBuf Signal() messages
/// @author Tor Slettnes <tor@slett.net>
///
/// Class templates for Common Core protobuf services (client and server), including:
///  * Settings store in YourServiceName.json, using JsonCpp as backend
///  * Status/error code wrappers
//==============================================================================

#pragma once
#include "thread/signaltemplate.h++"
#include "platform/symbols.h++"
#include "platform/init.h++"
#include "cc/protobuf/core/signal.pb.h"  // Generated from `signal.proto`

#include <functional>
#include <unordered_set>

namespace protobuf
{
    //==========================================================================
    /// @brief
    ///    Capture local signals and encode as Protobuf Signal() messages for
    ///    remote peers
    ///
    /// @tparam ProtoT
    ///     ProtoBuf Signal message
    ///

    template <class ProtoT>
    class SignalForwarder
    {
        using This = SignalForwarder;

    public:
        SignalForwarder()
            : signal_shutdown_handle(TYPE_NAME_FULL(This))
        {
        }

        // Implementations should override this in order to connect specific
        // Signal<T> or MappingSignal<T> instances to corresponding handlers,
        // which in turn will encode the payload and add the result to this
        // queue.
        virtual void initialize()
        {
            core::platform::signal_shutdown.connect(
                this->signal_shutdown_handle,
                std::bind(&This::deinitialize, this));
        }

        // Implementations should override this in order to disconnect any
        // signal handlers that were connected in their `initialize()` method.
        virtual void deinitialize()
        {
            core::platform::signal_shutdown.disconnect(
                this->signal_shutdown_handle);
        }

        // Implementations shold override this to propagate the encoded message
        // to its peers.
        virtual void forward(ProtoT &&message) = 0;

    protected:
        static cc::signal::MappingAction boolean_mapping(bool present)
        {
            return present ? cc::signal::MAP_UPDATE : cc::signal::MAP_REMOVAL;
        }

        static cc::signal::MappingAction mapping_action(core::signal::MappingAction action)
        {
            return static_cast<cc::signal::MappingAction>(action);
        }

        /// @brief
        ///    Create a new Signal message with optional mapping controls.
        static ProtoT create_signal_message()
        {
            return ProtoT();
        }

        /// @brief
        ///    Create a new Signal message with optional mapping controls.
        static ProtoT create_signal_message(
            core::signal::MappingAction mapping_action)
        {
            ProtoT msg;
            msg.set_mapping_action(This::mapping_action(mapping_action));
            return msg;
        }

        /// @brief
        ///    Create a new Signal message with optional mapping controls.
        static ProtoT create_signal_message(
            core::signal::MappingAction mapping_action,
            const std::string &mapping_key)
        {
            ProtoT msg = This::create_signal_message(mapping_action);
            msg.set_mapping_key(mapping_key);
            return msg;
        }

    private:
        core::signal::Handle signal_shutdown_handle;
    };
}  // namespace protobuf
