/// -*- c++ -*-
//==============================================================================
/// @file zmq-subscriber.c++
/// @brief Common functionality wrappers for ZeroMQ subscribers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-subscriber.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

#include <cstring>

namespace core::zmq
{
    Subscriber::Subscriber(const std::string &address,
                           const std::string &channel_name,
                           Role role)
        : Super(address, "subscriber", channel_name, ZMQ_SUB, role),
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

    void Subscriber::add_handler(const std::shared_ptr<MessageHandler> &shared_handler)
    {
        std::lock_guard lock(this->mtx_);
        this->init_handler(shared_handler);
        this->shared_handlers_.insert_or_assign(
            shared_handler->id(),
            shared_handler);
    }

    void Subscriber::register_handler(const std::weak_ptr<MessageHandler> &weak_handler)
    {
        if (auto shared_handler = weak_handler.lock())
        {
            std::lock_guard lock(this->mtx_);
            this->weak_handlers_.insert_or_assign(
                shared_handler->id(),
                weak_handler);
        }
    }

    void Subscriber::remove_handler(const std::shared_ptr<MessageHandler> &shared_handler)
    {
        std::lock_guard lock(this->mtx_);
        this->shared_handlers_.erase(shared_handler->id());
        this->deinit_handler(shared_handler);
    }

    void Subscriber::unregister_handler(const std::weak_ptr<MessageHandler> &weak_handler)
    {
        if (auto shared_handler = weak_handler.lock())
        {
            std::lock_guard lock(this->mtx_);
            this->weak_handlers_.erase(shared_handler->id());
        }
    }

    void Subscriber::clear()
    {
        std::lock_guard lock(this->mtx_);
        for (std::shared_ptr<MessageHandler> handler : this->handlers())
        {
            this->deinit_handler(handler);
        }
        this->shared_handlers_.clear();
        this->weak_handlers_.clear();
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
                       this->address());
            while (this->keep_receiving)
            {
                if (auto bytes = this->receive())
                {
                    this->process_message(*bytes);
                }
            }
        }
        catch (const Error &e)
        {
            this->log_zmq_error("could not continue receiving publications", e);
            this->keep_receiving = false;
        }
    }

    void Subscriber::process_message(const types::ByteVector &bytes)
    {
        std::lock_guard lock(this->mtx_);

        for (const std::shared_ptr<MessageHandler> &handler : this->handlers())
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
        this->setsockopt(ZMQ_SUBSCRIBE, filter.data(), filter.size());
    }

    void Subscriber::deinit_handler(const std::shared_ptr<MessageHandler> &handler)
    {
        logf_debug("%s removing subscription for %r with filter %r",
                   *this,
                   handler->id(),
                   handler->filter());
        try
        {
            const Filter &filter = handler->filter();
            this->check_error(::zmq_setsockopt(
                this->socket(),
                ZMQ_UNSUBSCRIBE,
                filter.data(),
                filter.size()));
        }
        catch (const Error &e)
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

    std::vector<std::shared_ptr<MessageHandler>> Subscriber::handlers()
    {
        std::vector<std::shared_ptr<MessageHandler>> handlers;
        handlers.reserve(this->shared_handlers_.size() + this->weak_handlers_.size());

        for (const auto &[id, shared_handler]: this->shared_handlers_)
        {
            handlers.push_back(shared_handler);
        }

        for (auto it = this->weak_handlers_.begin();
             it != this->weak_handlers_.end();
            )
        {
            if (std::shared_ptr<MessageHandler> shared_handler = it->second.lock())
            {
                handlers.push_back(shared_handler);
                ++it;
            }
            else
            {
                it = this->weak_handlers_.erase(it);
            }
        }
        return handlers;
    }

}  // namespace core::zmq
