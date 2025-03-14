// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-signalqueue.c++
/// @brief Base class for Switchboard servers (standalone or relay)
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "switchboard-grpc-signalqueue.h++"
#include "switchboard-signals.h++"

#include "protobuf-switchboard-types.h++"

namespace switchboard::grpc
{
    void SignalQueue::initialize()
    {
        using cc::switchboard::Signal;

        this->connect<switchboard::Specification>(
            Signal::kSpecification,
            switchboard::signal_spec,
            [](const Specification &spec, Signal *msg) {
                ::protobuf::encode(spec, msg->mutable_specification());
            });

        this->connect<switchboard::Status>(
            Signal::kStatus,
            switchboard::signal_status,
            [](const Status &status, Signal *msg) {
                ::protobuf::encode(status, msg->mutable_status());
            });

        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(switchboard::signal_status);
        this->disconnect(switchboard::signal_spec);
        Super::deinitialize();
    }
}  // namespace switchboard::grpc
