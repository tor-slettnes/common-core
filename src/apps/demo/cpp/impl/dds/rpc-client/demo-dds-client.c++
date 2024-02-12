// -*- c++ -*-
//==============================================================================
/// @file demo-dds-client.c++
/// @brief Demo DDS client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

// Application-specific modules
#include "demo-dds-client.h++"
#include "translate-idl-demo.h++"  // Demo data encode/decode routines

// Shared modules
#include "translate-idl-inline.h++"  // `encoded<>` and `decoded<>` templates
#include "platform/symbols.h++"

namespace demo::dds
{
    ClientImpl::ClientImpl(const std::string &identity, int domain_id)
        : API(identity, "DDS Client"),
          Client_Wrapper(CC::Demo::SERVICE_ID, domain_id)
    {
    }

    void ClientImpl::initialize()
    {
        Super::initialize();
        this->client().wait_for_service();
    }

    void ClientImpl::say_hello(const Greeting &greeting)
    {
        auto idl_greeting = idl::encoded<CC::Demo::Greeting>(greeting);
        this->client().say_hello(idl_greeting);
    }

    TimeData ClientImpl::get_current_time()
    {
        CC::Demo::TimeData idl_time_data = this->client().get_current_time();
        return idl::decoded<TimeData>(idl_time_data);
    }

    void ClientImpl::start_ticking()
    {
        this->client().start_ticking();
    }

    void ClientImpl::stop_ticking()
    {
        this->client().stop_ticking();
    }

    void ClientImpl::start_watching()
    {
        if (!this->subscriber)
        {
            logf_debug("Creating DDS demo subscriber");
            this->subscriber = Subscriber::create_shared(
                this->identity(),
                this->client().domain_id());

            this->subscriber->initialize();
        }
    }

    void ClientImpl::stop_watching()
    {
        if (this->subscriber)
        {
            logf_debug("Deinitializing DDS demo subscriber");
            this->subscriber->deinitialize();
            this->subscriber.reset();
        }
    }

}  // namespace demo::dds
