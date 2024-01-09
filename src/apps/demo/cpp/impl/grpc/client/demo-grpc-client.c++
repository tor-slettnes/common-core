// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-client.c++
/// @brief Demo gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-grpc-client.h++"
#include "demo-api.h++"
#include "demo-signals.h++"
#include "translate-protobuf-demo.h++"
#include "translate-protobuf-inline.h++"

namespace cc::demo::grpc
{
    void ClientImpl::initialize()
    {
        ClientImplBase::initialize();

        // Add handlers to be invoked by `SignalClientImpl<>::handleSignal()`
        // whenever a `Signal` message is received from the server.
        // These will re-emit the decoded payload as signals within this client
        // process.

        this->addHandler(
            CC::Demo::Signal::kGreeting,
            [&](const CC::Demo::Signal &signal) {
                signal_greeting.emit(
                    static_cast<signal::MappingChange>(signal.change()),
                    signal.key(),
                    protobuf::decoded<Greeting>(signal.greeting()));
            });

        this->addHandler(
            CC::Demo::Signal::kTime,
            [](const CC::Demo::Signal &signal) {
                signal_time.emit(
                    protobuf::decoded<TimeData>(signal.time()));
            });
    }

    void ClientImpl::say_hello(const Greeting &greeting)
    {
        this->call_check(
            &Stub::say_hello,
            protobuf::encoded<CC::Demo::Greeting>(greeting));
    }

    TimeData ClientImpl::get_current_time()
    {
        return protobuf::decoded<TimeData>(
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

}  // namespace cc::demo::grpc
