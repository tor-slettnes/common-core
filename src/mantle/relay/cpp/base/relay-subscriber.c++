// -*- c++ -*-
//==============================================================================
/// @file relay-types.h++
/// @brief Relay abstract base
/// @author Tor Slettnes
//==============================================================================

#include "relay-subscriber.h++"

namespace relay
{
    //--------------------------------------------------------------------------
    // Abstract Base

    void Subscriber::deinitialize()
    {
        this->stop_reader();
    }

    void Subscriber::subscribe(const std::string &handle,
                              const std::vector<std::string> &topics,
                              const MessageReceiver &receiver)
    {
        using namespace std::placeholders;
        signal_message.connect(
            handle,
            [=](core::signal::MappingAction action,
                const std::string &key,
                const core::types::Value &value) {
                receiver(key, value);
            });

        this->start_reader();
    }

    void Subscriber::unsubscribe(const std::string &handle)
    {
        signal_message.disconnect(handle);
        if (relay::signal_message.connection_count() == 0)
        {
            this->stop_reader();
        }
    }

}  // namespace relay
