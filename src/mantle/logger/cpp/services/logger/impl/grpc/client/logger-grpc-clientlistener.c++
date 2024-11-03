// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-clientlistener.c++
/// @brief Listen to log events from server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-grpc-clientlistener.h++"

namespace logger::grpc
{
    std::shared_ptr<EventQueue> ClientListener::listen(const ListenerSpec &spec)
    {
        return {};
    }
}  // namespace logger::grpc
