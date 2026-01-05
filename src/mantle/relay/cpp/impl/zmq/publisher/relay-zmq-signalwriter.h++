// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-signalwriter.h++
/// @brief Forward local RELAY signals over ZMQ
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-common.h++"
#include "zmq-messagewriter.h++"
#include "types/create-shared.h++"

namespace relay::zmq
{
    //==========================================================================
    // @class SignalWriter
    // @brief Connect to local RELAY signals and write via ZMQ

    class SignalWriter : public core::zmq::MessageWriter,
                         public core::types::enable_create_shared<SignalWriter>
    {
        // Convencience alias
        using This = SignalWriter;
        using Super = core::zmq::MessageWriter;

    protected:
        using Super::Super;

    public:
        void initialize() override;
        void deinitialize() override;

    private:
        void write_message(
            core::signal::MappingAction action,
            const std::string &topic,
            const core::types::Value &payload);
    };
}  // namespace relay::zmq
