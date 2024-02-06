/// -*- c++ -*-
//==============================================================================
/// @file zmq-subscriber.c++
/// @brief Common functionality wrappers for ZeroMQ subscribers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-subscriber.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace cc::zmq
{
    Subscriber::Subscriber(const std::string &host_address,
                           const std::string &channel_name)
        : Super(host_address,
                "subscriber",
                channel_name,
                ::zmq::socket_type::sub),
          keep_receiving(false)
    {
    }

    Subscriber::~Subscriber()
    {
        this->stop_receiving();
    }

    void Subscriber::subscribe(const Callback &callback)
    {
        this->subscribe({}, callback);
    }

    void Subscriber::subscribe(const Filter &filter,
                               const Callback &callback)
    {
        logf_trace("%s subscribing with filter %r", *this, filter.to_hex());
        this->socket()->set(::zmq::sockopt::subscribe, filter.stringview());
        this->subscriptions_[filter] = callback;
        this->start_receiving();
    }

    void Subscriber::subscribe_topic(const std::string &topic,
                                     const Callback &callback)
    {
        if (auto filter = Filter::create_from_topic(topic))
        {
            this->subscribe(*filter, callback);
        }
    }

    void Subscriber::unsubscribe()
    {
        this->unsubscribe({});
    }

    void Subscriber::unsubscribe(const Filter &filter)
    {
        this->subscriptions_.erase(filter);
        try
        {
            this->socket()->set(::zmq::sockopt::unsubscribe, filter.stringview());
        }
        catch (const ::zmq::error_t &e)
        {
            this->log_zmq_error("could not unsubscribe", e);
        }
    }

    void Subscriber::unsubscribe_topic(const std::string &topic)
    {
        if (auto filter = Filter::create_from_topic(topic))
        {
            this->unsubscribe(*filter);
        }
    }

    void Subscriber::start_receiving()
    {
        this->keep_receiving = true;
        if (!this->receive_thread.joinable())
        {
            this->receive_thread = std::thread(&Subscriber::receive_loop, this);
        }
    }

    void Subscriber::stop_receiving()
    {
        this->keep_receiving = false;
        if (std::thread t = std::move(this->receive_thread); t.joinable())
        {
            log_debug("Waiting for ZMQ subscriber thread");
            t.join();
        }
    }

    void Subscriber::receive_loop()
    {
        try
        {
            logf_debug("%s listening for publications from %s",
                       *this,
                       this->host_address());
            while (this->keep_receiving)
            {
                ::zmq::message_t msg;
                if (this->receive(&msg))
                {
                    this->process_zmq_message(msg);
                }
            }
        }
        catch (const ::zmq::error_t &e)
        {
            this->log_zmq_error("could not continue receiving publications", e);
            this->keep_receiving = false;
        }
    }

    void Subscriber::process_zmq_message(const ::zmq::message_t &msg)
    {
        for (const auto &[filter, callback] : this->subscriptions_)
        {
            if ((msg.size() >= filter.size()) &&
                (memcmp(msg.data(), filter.data(), filter.size()) == 0))
            {
                this->invoke_callback(callback, msg, filter);
            }
        }
    }

    void Subscriber::invoke_callback(const Callback &callback,
                                     const ::zmq::message_t &msg,
                                     const Filter &filter)
    {
        log_trace("Invoking ZMQ callback for filter: ", filter);
        auto *data = reinterpret_cast<const types::Byte *>(msg.data());

        switch (static_cast<CallbackSignature>(callback.index()))
        {
        case CB_ZMQ_MSG:
            std::get<CB_ZMQ_MSG>(callback)(msg);
            break;

        case CB_BYTES:
            std::get<CB_BYTES>(callback)(
                types::ByteVector(data + filter.size(),
                                 data + msg.size()));
            break;

        case CB_TOPIC_BYTES:
            std::get<CB_TOPIC_BYTES>(callback)(
                filter.topic(),
                types::ByteVector(data + filter.size(),
                                 data + msg.size()));
            break;

        default:
            break;
        }
    }
}  // namespace cc::zmq
