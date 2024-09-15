// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-client.c++
/// @brief Vfs gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-grpc-client.h++"
#include "vfs-remote-context.h++"
#include "vfs-signals.h++"
#include "protobuf-vfs-types.h++"
#include "protobuf-inline.h++"

namespace platform::vfs::grpc
{
    void Client::initialize()
    {
        Super::initialize();
        using namespace std::placeholders;

        this->add_mapping_handler(
            cc::platform::vfs::Signal::kContext,
            std::bind(&This::on_context, this, _1, _2, _3));

        this->add_mapping_handler(
            cc::platform::vfs::Signal::kContextInUse,
            std::bind(&This::on_context_in_use, this, _1, _2, _3)
    }

    void Client::on_context(
        core::signal::MappingAction action,
        const std::string &key,
        const cc::platform::vfs::Signal &signal) const
    {
        platform::vfs::signal_context.emit(
            action,
            key,
            this->decoded_context(signal.context()));
    }

    void Client::on_context_in_use(
        core::signal::MappingAction action,
        const std::string &key,
        const cc::platform::vfs::Signal &signal) const
    {
        platform::vfs::signal_context_in_use.emit(
            action,
            key,
            this->decoded_context(signal.context_in_use()));
    }

}  // namespace platform::vfs::grpc
