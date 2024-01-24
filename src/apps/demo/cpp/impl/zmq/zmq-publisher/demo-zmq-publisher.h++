// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-publisher.h++
/// @brief Publish demo signals over ZeroMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-types.h++"
#include "demo-zmq-common.h++"
#include "zmq-protobuf-publisher.h++"

#include "types/create-shared.h++"
#include "thread/signaltemplate.h++"

namespace cc::demo::zmq
{
    //==========================================================================
    // @class Publisher
    // @brief Publish locally-emitted demo signals over ZeroMQ

    class Publisher : public cc::zmq::ProtoBufPublisher,
                      public cc::types::enable_create_shared<Publisher>
    {
        // Convencience alias
        using This = Publisher;
        using Super = cc::zmq::ProtoBufPublisher;

    protected:
        Publisher(const std::string &channel_name = CHANNEL_NAME,
                  const std::string &bind_address = "");

    public:
        void initialize() override;
        void deinitialize() override;

    private:
        void on_time_update(const TimeData &time_data);

        void on_greeting_update(cc::signal::MappingChange change,
                                const std::string &identity,
                                const Greeting &greeting);
    };
}  // namespace cc::demo::zmq
