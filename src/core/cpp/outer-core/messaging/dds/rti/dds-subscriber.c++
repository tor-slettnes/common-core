/// -*- c++ -*-
//==============================================================================
/// @file dds-subscriber.c++
/// @brief A subscribing entity/consumer in a DDS environment
/// @author Tor Slettnes
//==============================================================================

#include "dds-subscriber.h++"
#include "logging/logging.h++"
#include "platform/symbols.h++"

namespace core::dds
{
    Subscriber::Subscriber(const std::string &channel_name, int domain_id)
        : Super("subscriber", channel_name, domain_id),
          ::dds::sub::Subscriber(this->get_participant()),
          keep_listening(false)
    {
    }

    Subscriber::~Subscriber()
    {
        this->stop_listening();
    }

    void Subscriber::initialize()
    {
        Super::initialize();
        this->start_listening();
    }

    void Subscriber::deinitialize()
    {
        this->stop_listening();
        Super::initialize();
    }

    void Subscriber::start_listening()
    {
        this->keep_listening = true;
        if (!this->listen_thread.joinable())
        {
            logf_debug("Starting listener thread");
            this->listen_thread = std::thread(&Subscriber::listen, this);
        }
    }

    void Subscriber::stop_listening()
    {
        this->keep_listening = false;
        if (this->listen_thread.joinable())
        {
            logf_debug("Shutting down listener thread");
            this->listen_thread.join();
            logf_debug("Exited listener thread");
        }
    }

    void Subscriber::listen()
    {
        while (this->keep_listening)
        {
            this->waitset.dispatch(std::chrono::seconds(1));
        }
    }

}  // namespace core::dds
