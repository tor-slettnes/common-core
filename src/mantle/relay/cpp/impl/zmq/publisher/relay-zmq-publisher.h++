// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-publisher.h++
/// @brief Publish relay signals over ZeroMQ
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-zmq-signalwriter.h++"
#include "relay-zmq-common.h++"
#include "relay-publisher.h++"
#include "zmq-publisher.h++"
#include "types/create-shared.h++"

namespace relay::zmq
{
    //==========================================================================
    // @class Publisher
    // @brief Publish locally-emitted relay signals over ZeroMQ

    class Publisher : public relay::Publisher,
                      public core::zmq::Publisher,
                      public core::types::enable_create_shared_from_this<Publisher>
    {
        // Convencience alias
        using This = Publisher;
        using Super = core::zmq::Publisher;

    protected:
        Publisher(const std::string &bind_address = "",
                  const std::string &channel_name = PRODUCER_CHANNEL,
                  Role role = Role::SATELLITE);

    public:
        void initialize() override;
        void deinitialize() override;
        bool publish(const std::string &topic,
                     const core::types::Value &payload) override;


    private:
        std::shared_ptr<SignalWriter> signal_writer;
    };
}  // namespace relay::zmq
