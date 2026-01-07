// -*- c++ -*-
//==============================================================================
/// @file relay-types.h++
/// @brief Relay abstract base
/// @author Tor Slettnes
//==============================================================================

#include "relay-subscriber.h++"

namespace pubsub
{
    //--------------------------------------------------------------------------
    // Abstract Base

    void Subscriber::deinitialize()
    {
        this->stop_reader();
    }

    void Subscriber::subscribe(const std::string &handle,
                               const TopicSet &topics,
                               const MessageHandler &handler)
    {
        using namespace std::placeholders;
        signal_publication.connect(
            handle,
            [=](core::signal::MappingAction action,
                const std::string &key,
                const core::types::Value &value) {
                if (topics.empty() || topics.count(key))
                {
                    handler(key, value);
                }
            });

        this->start_reader();
    }

    void Subscriber::unsubscribe(const std::string &handle)
    {
        signal_publication.disconnect(handle);
        if (pubsub::signal_publication.connection_count() == 0)
        {
            this->stop_reader();
        }
    }

}  // namespace pubsub
