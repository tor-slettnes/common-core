/// -*- c++ -*-
//==============================================================================
/// @file dds-subscriber.c++
/// @brief A subscribing entity/consumer in a DDS environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dds-subscriber.h++"
#include "logging/logging.h++"

namespace cc::dds
{
    DDS_Subscriber::DDS_Subscriber(
        const std::string &type,
        const std::string &name,
        int domain_id)
        : Super(type, name, domain_id),
          ::dds::sub::Subscriber(this->get_participant()),
          keep_listening(false)
    {
        log_trace("DDS_Subscriber() constructor");
    }

    DDS_Subscriber::~DDS_Subscriber()
    {
        this->stop_listening();
    }

    void DDS_Subscriber::initialize()
    {
        Super::initialize();
        this->start_listening();
    }

    void DDS_Subscriber::deinitialize()
    {
        this->stop_listening();
        Super::initialize();
    }

    void DDS_Subscriber::start_listening()
    {
        this->keep_listening = true;
        if (!this->listen_thread.joinable())
        {
            logf_debug("Starting listener thread");
            this->listen_thread = std::thread(&DDS_Subscriber::listen, this);
        }
    }

    void DDS_Subscriber::stop_listening()
    {
        this->keep_listening = false;
        if (this->listen_thread.joinable())
        {
            logf_debug("Shutting down listener thread");
            this->listen_thread.join();
            logf_debug("Exited listener thread");
        }
    }

    void DDS_Subscriber::listen()
    {
        while (this->keep_listening)
        {
            this->waitset.dispatch(std::chrono::seconds(1));
        }
    }

}  // namespace cc::dds
