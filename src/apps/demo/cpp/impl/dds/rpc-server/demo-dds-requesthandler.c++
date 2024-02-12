// -*- c++ -*-
//==============================================================================
/// @file demo-dds-requesthandler.c++
/// @brief Handle DemoService RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-dds-requesthandler.h++"
#include "demo-signals.h++"
#include "translate-idl-demo.h++"
#include "translate-idl-inline.h++"

#include "logging/logging.h++"

namespace demo::dds
{
    RequestHandler::RequestHandler(const std::shared_ptr<API> &api_provider)
        : provider(api_provider)
    {
        logf_debug("Demo DDS RequestHandler Constructor");
    }

    void RequestHandler::say_hello(const CC::Demo::Greeting &greeting)
    {
        // We received a greeting from a client.  Emit a signal to registered
        // callbacks (slots). (This includes `Publisher::on_signal_greeting()`,
        // which then forwards the signals to clients as a published message.)
        this->provider->say_hello(idl::decoded<Greeting>(greeting));
    }

    CC::Demo::TimeData RequestHandler::get_current_time()
    {
        // We received a request for current time data.  Obtain this from our
        // own provider, which is normally the `native` implemenentation (unless
        // we are running as a proxy, in which case `provider` could be a client
        // that cascades this request to the real server).
        TimeData time_data = this->provider->get_current_time();

        logf_debug("Responding to time request: %s", time_data);

        // Encode the native TimeData structure to a CC::Demo TimeData structure
        // in the reply.
        return idl::encoded<CC::Demo::TimeData>(time_data);
    }

    void RequestHandler::start_ticking()
    {
        // Start scheduled task to publish TimeData updates at regular intervals
        this->provider->start_ticking();
    }

    void RequestHandler::stop_ticking()
    {
        // Stop scheduled task to publish TimeData updates
        this->provider->stop_ticking();
    }

}  // namespace demo::dds
