// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-client.c++
/// @brief Demo gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-grpc-client.h++"
#include "demo-api.h++"
#include "demo-signals.h++"
#include "protobuf-demo-types.h++"
#include "protobuf-message.h++"
#include "protobuf-inline.h++"

namespace demo::grpc
{
    void ClientImpl::initialize()
    {
        ClientImplBase::initialize();

        // Add handlers to be invoked by `SignalClientImpl<>::handleSignal()`
        // whenever a `Signal` message is received from the server.
        // These will re-emit the decoded payload as signals within this client
        // process.

        this->add_handler(
            cc::protobuf::demo::Signal::kSignalGreeting,
            [&](const cc::protobuf::demo::Signal &signal) {
                signal_greeting.emit(
                    static_cast<cc::signal::MappingAction>(signal.mapping_action()),
                    signal.mapping_key(),
                    ::cc::io::proto::decoded<Greeting>(signal.signal_greeting()));
            });

        this->add_handler(
            cc::protobuf::demo::Signal::kSignalTime,
            [](const cc::protobuf::demo::Signal &signal) {
                signal_time.emit(
                    ::cc::io::proto::decoded<TimeData>(signal.signal_time()));
            });
    }

    void ClientImpl::say_hello(const Greeting &greeting)
    {
        this->call_check(
            &Stub::say_hello,
            ::cc::io::proto::encoded<cc::protobuf::demo::Greeting>(greeting));
    }

    TimeData ClientImpl::get_current_time()
    {
        return ::cc::io::proto::decoded<TimeData>(
            this->call_check(&Stub::get_current_time));
    }

    void ClientImpl::start_ticking()
    {
        this->call_check(
            &Stub::start_ticking);
    }

    void ClientImpl::stop_ticking()
    {
        this->call_check(
            &Stub::stop_ticking);
    }

    void ClientImpl::start_watching()
    {
        ClientImplBase::start_watching();
    }

    void ClientImpl::stop_watching()
    {
        ClientImplBase::stop_watching();
    }

}  // namespace demo::grpc
