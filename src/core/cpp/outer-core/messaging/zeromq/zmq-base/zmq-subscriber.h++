/// -*- c++ -*-
//==============================================================================
/// @file zmq-subscriber.h++
/// @brief Common functionality wrappers for ZeroMQ subscribers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-endpoint.h++"
#include "zmq-messagehandler.h++"

#include <thread>
#include <set>
#include <mutex>

namespace core::zmq
{
    class Subscriber : public Endpoint
    {
        using This = Subscriber;
        using Super = Endpoint;

    public:
        Subscriber(const std::string &address,
                   const std::string &channel_name,
                   Role role = Role::SATELLITE);

        ~Subscriber();

    public:
        void initialize() override;
        void deinitialize() override;

    public:
        void add_handler(const std::shared_ptr<MessageHandler> &handler,
                         bool initialize = false);

        void remove_handler(const std::shared_ptr<MessageHandler> &handler,
                            bool deinitialize = false);

        void clear(bool deinitialize = true);

    private:
        void start_receiving();
        void stop_receiving();
        void receive_loop();
        void process_message(const MessageParts &parts);

        void add_handler_filter(const std::shared_ptr<MessageHandler> &handler);
        void remove_handler_filter(const std::shared_ptr<MessageHandler> &handler);
        void invoke_handler(const std::shared_ptr<MessageHandler> &handler,
                            const MessageParts &parts);

    private:
        std::recursive_mutex mtx_;
        std::set<std::shared_ptr<MessageHandler>> handlers_;
        std::thread receive_thread;
        bool keep_receiving;
    };

}  // namespace core::zmq
