// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-publisher.h++
/// @brief Publish demo signals over ZeroMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-zmq-signalwriter.h++"
#include "demo-zmq-common.h++"
#include "demo-types.h++"
#include "zmq-publisher.h++"

#include "types/create-shared.h++"
#include "thread/signaltemplate.h++"

namespace demo::zmq
{
    //==========================================================================
    // @class Publisher
    // @brief Publish locally-emitted demo signals over ZeroMQ

    class Publisher : public shared::zmq::Publisher,
                      public shared::types::enable_create_shared_from_this<Publisher>
    {
        // Convencience alias
        using This = Publisher;
        using Super = shared::zmq::Publisher;

    protected:
        Publisher(const std::string &bind_address = "",
                  const std::string &channel_name = MESSAGE_CHANNEL);

    public:
        void initialize() override;
        void deinitialize() override;

    private:
        std::shared_ptr<SignalWriter> signal_writer;
    };
}  // namespace demo::zmq
