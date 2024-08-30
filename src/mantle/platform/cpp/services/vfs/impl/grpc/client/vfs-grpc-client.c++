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

        this->add_mapping_handler(
            cc::platform::vfs::Signal::kContext,
            &This::on_context);

        this->add_mapping_handler(
            cc::platform::vfs::Signal::kContextInUse,
            &This::on_context_in_use);
    }

    void Client::on_context(
        core::signal::MappingAction action,
        const std::string &key,
        const cc::platform::vfs::Signal &signal)
    {
        platform::vfs::signal_context.emit(
            action,
            key,
            protobuf::decode_shared<RemoteContext>(signal.context()));
    }

    void Client::on_context_in_use(
        core::signal::MappingAction action,
        const std::string &key,
        const cc::platform::vfs::Signal &signal)
    {
        platform::vfs::signal_context_in_use.emit(
            action,
            key,
            protobuf::decode_shared<RemoteContext>(signal.context_in_use()));
    }

}  // namespace platform::vfs::grpc
