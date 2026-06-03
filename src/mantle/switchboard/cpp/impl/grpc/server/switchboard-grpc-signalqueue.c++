// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-signalqueue.c++
/// @brief BlockingQueue to forward signals to connected client
/// @author Tor Slettnes
//==============================================================================

#include "switchboard-grpc-signalqueue.h++"
#include "switchboard-signals.h++"

#include "protobuf-switchboard-types.h++"
#include "protobuf-message.h++"

namespace cc::platform::switchboard::grpc
{
    void SignalQueue::initialize()
    {
        using cc::platform::switchboard::protobuf::Signal;

        this->connect<switchboard::Specification>(
            Signal::kSpecification,
            switchboard::signal_spec,
            [](const Specification &spec, Signal *msg) {
                cc::protobuf::encode(spec, msg->mutable_specification());
            });

        this->connect<switchboard::Status>(
            Signal::kStatus,
            switchboard::signal_status,
            [](const Status &status, Signal *msg) {
                cc::protobuf::encode(status, msg->mutable_status());
            });

        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(switchboard::signal_status);
        this->disconnect(switchboard::signal_spec);
        Super::deinitialize();
    }
}  // namespace cc::platform::switchboard::grpc
