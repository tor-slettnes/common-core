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

        this->add_mapping_handler(
            cc::demo::Signal::kSignalGreeting,
            [&](core::signal::MappingAction action,
                const std::string &key,
                const cc::demo::Signal &signal) {
                signal_greeting.emit(
                    action,
                    key,
                    ::protobuf::decoded<Greeting>(signal.signal_greeting()));
            });

        this->add_handler(
            cc::demo::Signal::kSignalTime,
            [](const cc::demo::Signal &signal) {
                signal_time.emit(
                    ::protobuf::decoded<TimeData>(signal.signal_time()));
            });
    }

    void ClientImpl::say_hello(const Greeting &greeting)
    {
        this->call_check(
            &Stub::SayHello,
            ::protobuf::encoded<cc::demo::Greeting>(greeting));
    }

    TimeData ClientImpl::get_current_time()
    {
        return ::protobuf::decoded<TimeData>(
            this->call_check(&Stub::GetCurrentTime));
    }

    void ClientImpl::start_ticking()
    {
        this->call_check(
            &Stub::StartTicking);
    }

    void ClientImpl::stop_ticking()
    {
        this->call_check(
            &Stub::StopTicking);
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
