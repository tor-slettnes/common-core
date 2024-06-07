/// -*- c++ -*-
//==============================================================================
/// @file zmq-subscriber.c++
/// @brief Common functionality wrappers for ZeroMQ subscribers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-subscriber.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace core::zmq
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

    void Subscriber::initialize()
    {
        Super::initialize();
        this->start_receiving();
    }

    void Subscriber::deinitialize()
    {
        this->clear();
        Super::deinitialize();
    }

    void Subscriber::add(std::shared_ptr<MessageHandler> handler)
    {
        std::lock_guard lock(this->mtx_);
        this->init_handler(handler);
        this->handlers_.insert(handler);
    }

    void Subscriber::remove(std::shared_ptr<MessageHandler> handler)
    {
        std::lock_guard lock(this->mtx_);
        this->handlers_.erase(handler);
        this->deinit_handler(handler);
    }

    void Subscriber::clear()
    {
        std::lock_guard lock(this->mtx_);
        for (std::shared_ptr<MessageHandler> handler : this->handlers_)
        {
            this->deinit_handler(handler);
        }
        this->handlers_.clear();
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
                if (auto bytes = this->receive())
                {
                    this->process_message(*bytes);
                }
            }
        }
        catch (const ::zmq::error_t &e)
        {
            this->log_zmq_error("could not continue receiving publications", e);
            this->keep_receiving = false;
        }
    }

    void Subscriber::process_message(const types::ByteVector &bytes)
    {
        std::lock_guard lock(this->mtx_);

        for (const std::shared_ptr<MessageHandler> &handler : this->handlers_)
        {
            const Filter &filter = handler->filter();
            if ((bytes.size() >= filter.size()) &&
                (memcmp(bytes.data(), filter.data(), filter.size()) == 0))
            {
                this->invoke_handler(handler, bytes);
            }
        }
    }

    void Subscriber::init_handler(const std::shared_ptr<MessageHandler> &handler)
    {
        handler->initialize();
        const Filter &filter = handler->filter();
        logf_debug("%s adding subscription for %r with filter %r",
                   *this,
                   handler->id(),
                   filter);
        this->socket()->set(::zmq::sockopt::subscribe,
                            filter.stringview());
    }

    void Subscriber::deinit_handler(const std::shared_ptr<MessageHandler> &handler)
    {
        logf_debug("%s Removing subscription for %r with filter %r",
                   *this,
                   handler->id(),
                   handler->filter());
        try
        {
            this->socket()->set(::zmq::sockopt::unsubscribe,
                                handler->filter().stringview());
        }
        catch (const ::zmq::error_t &e)
        {
            this->log_zmq_error("could not unsubscribe", e);
        }
        handler->deinitialize();
    }

    void Subscriber::invoke_handler(const std::shared_ptr<MessageHandler> &handler,
                                    const types::ByteVector &data)
    {
        logf_trace("%s invoking handler %r, data=%r",
                   *this,
                   handler->id(),
                   data);
        try
        {
            handler->handle_raw(data);
        }
        catch (...)
        {
            logf_warning("%s handler %r failed to handle ZMQ message {data=%s}: %s",
                         *this,
                         handler->id(),
                         data,
                         std::current_exception());
        }
    }

}  // namespace core::zmq
