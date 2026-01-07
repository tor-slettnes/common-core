// -*- c++ -*-
//==============================================================================
/// @file relay-publisher.h++
/// @brief Relay publisher abstract base
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-types.h++"
#include "thread/blockingqueue.h++"

namespace pubsub
{
    //--------------------------------------------------------------------------
    // Abstract Publisher Interface

    class Publisher
    {
        using This = Publisher;

    protected:
        Publisher();

    public:
        virtual void initialize();
        virtual void deinitialize();
        void publish(const std::string &topic,
                     const core::types::Value &payload);

    protected:
        virtual void start_writer();
        virtual void stop_writer();
        void write_worker();

    protected:
        virtual bool write(const std::string &topic,
                           const core::types::Value &payload) = 0;

    private:
        std::thread writer_thread_;
        core::types::BlockingQueue<MessageItem> writer_queue_;
    };

}  // namespace pubsub
