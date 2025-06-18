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
#include <unordered_set>
#include <mutex>

namespace core::zmq
{
    class Subscriber : public Endpoint
    {
        using This = Subscriber;
        using Super = Endpoint;

    protected:
        Subscriber(const std::string &address,
                   const std::string &channel_name,
                   Role role = Role::SATELLITE);

        ~Subscriber();

    public:
        void initialize() override;
        void deinitialize() override;

    public:
        void add(std::shared_ptr<MessageHandler> handler);
        void remove(std::shared_ptr<MessageHandler> handler);
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

    private:
        std::recursive_mutex mtx_;
        std::unordered_set<std::shared_ptr<MessageHandler>> handlers_;
        std::thread receive_thread;
        bool keep_receiving;
    };

}  // namespace core::zmq
