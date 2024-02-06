/// -*- c++ -*-
//==============================================================================
/// @file zmq-subscriber.h++
/// @brief Common functionality wrappers for ZeroMQ subscribers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-satellite.h++"
#include "zmq-filter.h++"
#include "types/bytevector.h++"
#include "types/valuemap.h++"
#include "platform/symbols.h++"
#include "logging/logging.h++"

#include <thread>
#include <functional>
#include <variant>

namespace cc::zmq
{
    class Subscriber : public Satellite
    {
        using This = Subscriber;
        using Super = Satellite;

        using Callback = std::variant<
            std::function<void(const ::zmq::message_t &)>,
            std::function<void(const types::ByteVector &)>,
            std::function<void(const std::string &topic, const types::ByteVector &)>>;

        enum CallbackSignature
        {
            CB_ZMQ_MSG,
            CB_BYTES,
            CB_TOPIC_BYTES,
        };

    protected:
        Subscriber(const std::string &host_address,
                   const std::string &channel_name);

        ~Subscriber();

    public:
        void subscribe(const Callback &callback);

        void subscribe(const Filter &filter,
                       const Callback &callback);

        void subscribe_topic(const std::string &topic,
                             const Callback &callback);

        void unsubscribe();

        void unsubscribe(const Filter &filter);

        void unsubscribe_topic(const std::string &topic);

    private:
        void start_receiving();
        void stop_receiving();
        void receive_loop();

    protected:
        virtual void process_zmq_message(const ::zmq::message_t &msg);
        virtual void invoke_callback(const Callback &callback,
                                     const ::zmq::message_t &msg,
                                     const Filter &filter);

    private:
        std::map<Filter, Callback> subscriptions_;
        std::thread receive_thread;
        bool keep_receiving;
    };

}  // namespace cc::zmq
