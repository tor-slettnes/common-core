// -*- c++ -*-
//==============================================================================
/// @file relay-subscriber.h++
/// @brief Relay subscriber abstract base
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-types.h++"

namespace pubsub
{
    //--------------------------------------------------------------------------
    // Abstract Interface

    class Subscriber
    {
        using This = Subscriber;

    public:
        virtual void initialize() {}
        virtual void deinitialize();

    public:
        void subscribe(const std::string &handle,
                       const TopicSet &topics,
                       const MessageHandler &handler);

        void unsubscribe(const std::string &handle);

    protected:
        virtual void start_reader() {}
        virtual void stop_reader() {}
    };

}  // namespace pubsub
