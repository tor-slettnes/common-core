/// -*- c++ -*-
//==============================================================================
/// @file zmq-subscriber.h++
/// @brief Common functionality wrappers for ZeroMQ subscribers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-endpoint.h++"
#include "zmq-messagehandler.h++"
#include "zmq-filter.h++"

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
        void add_handler(const std::shared_ptr<MessageHandler> &handler);
        void register_handler(const std::weak_ptr<MessageHandler> &handler);

        void remove_handler(const std::shared_ptr<MessageHandler> &handler);
        void unregister_handler(const std::weak_ptr<MessageHandler> &handler);
        void clear();

    private:
        void start_receiving();
        void stop_receiving();
        void receive_loop();
        void process_message(const types::ByteVector &bytes);

        void init_handler(const std::shared_ptr<MessageHandler> &handler);
        void deinit_handler(const std::shared_ptr<MessageHandler> &handler);
        void invoke_handler(const std::shared_ptr<MessageHandler> &handler,
                            const types::ByteVector &data);

        std::vector<std::shared_ptr<MessageHandler>> handlers();

    private:
        std::recursive_mutex mtx_;
        std::unordered_map<MessageHandler::Identity, std::shared_ptr<MessageHandler>> shared_handlers_;
        std::unordered_map<MessageHandler::Identity, std::weak_ptr<MessageHandler>> weak_handlers_;
        std::thread receive_thread;
        bool keep_receiving;
    };

}  // namespace core::zmq
