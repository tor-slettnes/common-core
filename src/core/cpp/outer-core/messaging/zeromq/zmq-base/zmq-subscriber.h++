/// -*- c++ -*-
//==============================================================================
/// @file zmq-subscriber.h++
/// @brief Common functionality wrappers for ZeroMQ subscribers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-satellite.h++"
#include "zmq-messagehandler.h++"
#include "zmq-filter.h++"

#include <thread>
#include <unordered_set>
#include <mutex>

namespace core::zmq
{
    class Subscriber : public Satellite
    {
        using This = Subscriber;
        using Super = Satellite;

    protected:
        Subscriber(const std::string &host_address,
                   const std::string &channel_name);

        ~Subscriber();

    public:
        void deinitialize() override;

    public:
        void add(std::shared_ptr<MessageHandler> handler);
        void remove(std::shared_ptr<MessageHandler> handler);
        void clear();

    private:
        void start_receiving();
        void stop_receiving();
        void receive_loop();

        void init_handler(const std::shared_ptr<MessageHandler> &handler);
        void deinit_handler(const std::shared_ptr<MessageHandler> &handler);
        void invoke_handler(const std::shared_ptr<MessageHandler> &handler,
                            const types::ByteVector &payload,
                            const Filter &filter);

        void process_message(const types::ByteVector &bytes);

    private:
        std::recursive_mutex mtx_;
        std::unordered_set<std::shared_ptr<MessageHandler>> handlers_;
        std::thread receive_thread;
        bool keep_receiving;
    };

}  // namespace core::zmq
