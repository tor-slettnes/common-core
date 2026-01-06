/// -*- c++ -*-
//==============================================================================
/// @file zmq-subscriber.c++
/// @brief Common functionality wrappers for ZeroMQ subscribers
/// @author Tor Slettnes
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
        this->keep_receiving = false;
        if (std::thread t = std::move(this->receive_thread); t.joinable())
        {
            t.detach();
        }
    }

    void Subscriber::add_handler(const std::shared_ptr<MessageHandler> &handler,
                                 bool initialize)
    {
        if (initialize)
        {
            handler->initialize();
        }

        // This has no effect (`libzmq3` bug?).  Instead, we use an empty filter above.
        // this->add_handler_filter(handler);

        {
            std::scoped_lock lock(this->mtx_);
            this->handlers_.insert(handler);
        }
    }

    void Subscriber::remove_handler(const std::shared_ptr<MessageHandler> &handler,
                                    bool deinitialize)
    {
        {
            std::scoped_lock lock(this->mtx_);
            this->handlers_.erase(handler);
        }

        // This has no effect (`libzmq3` bug?).  Instead, we use an empty filter above.
        // this->remove_handler_filter(handler);

        if (deinitialize)
        {
            handler->deinitialize();
        }
    }

    void Subscriber::clear(bool deinitialize)
    {
        std::scoped_lock lock(this->mtx_);
        for (auto handler : this->handlers_)
        {
            // This has no effect (`libzmq3` bug?). Instead, we use an empty filter above.
            // this->remove_handler_filter(handler);

            if (deinitialize)
            {
                logf_trace("%s deinitializing handler %r", *this, handler->id());
                handler->deinitialize();
            }
        }
        logf_trace("%s removing handlers", *this);
        this->handlers_.clear();
    }

    void Subscriber::start_listening()
    {
        this->keep_receiving = true;
        if (!this->receive_thread.joinable())
        {
            this->receive_thread = std::thread(&Subscriber::listen, this);
        }
    }

    void Subscriber::stop_listening()
    {
        this->keep_receiving = false;
    }

    void Subscriber::listen()
    {
        logf_debug("%s listening for publications from %s",
                   *this,
                   this->address());

        // Workaround for apparent `libzmq3` bug:
        // Setting a non-empty filter (see `add_handler_filter()` below) seems
        // to have no effect.  Instead, we must add an empty filter if we want
        // to receive messages at all.

        this->setsockopt(ZMQ_SUBSCRIBE, "", 0);
        this->keep_receiving = true;

        try
        {
            while (this->keep_receiving)
            {
                this->process_message(this->receive_parts());
            }

            logf_debug("%s is no longer listening for publications from %s",
                       *this,
                       this->address());

            this->setsockopt(ZMQ_UNSUBSCRIBE, "", 0);
        }
        catch (const Error &e)
        {
            this->log_zmq_error("could not continue receiving publications", e);
            this->keep_receiving = false;
        }
    }

    void Subscriber::process_message(const MessageParts &parts)
    {
        if (!parts.empty())
        {
            const core::types::ByteVector &header = parts.front();

            std::scoped_lock lock(this->mtx_);

            for (const std::shared_ptr<MessageHandler> &handler : this->handlers_)
            {
                if (!handler->filter() || (handler->filter().value() == header))
                {
                    this->invoke_handler(handler, parts);
                }
            }
        }
    }

    void Subscriber::add_handler_filter(const std::shared_ptr<MessageHandler> &handler)
    {
        if (const std::optional<Filter> &filter = handler->filter())
        {
            logf_debug("%s adding subscription for %r with filter %r",
                       *this,
                       handler->id(),
                       *filter);
            this->setsockopt(ZMQ_SUBSCRIBE, filter->data(), filter->size());
        }
        // this->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    }

    void Subscriber::remove_handler_filter(const std::shared_ptr<MessageHandler> &handler)
    {
        if (const std::optional<Filter> &filter = handler->filter())
        {
            logf_debug("%s removing subscription for %r with filter %r",
                       *this,
                       handler->id(),
                       *filter);
            try
            {
                this->check_error(::zmq_setsockopt(
                    this->socket(),
                    ZMQ_UNSUBSCRIBE,
                    filter->data(),
                    filter->size()));
            }
            catch (const Error &e)
            {
                this->log_zmq_error("could not unsubscribe", e);
            }
        }
    }

    void Subscriber::invoke_handler(const std::shared_ptr<MessageHandler> &handler,
                                    const MessageParts &parts)
    {
        logf_trace("%s invoking handler %r, parts=%r",
                   *this,
                   handler->id(),
                   parts);
        try
        {
            handler->handle(parts);
        }
        catch (...)
        {
            logf_error("%s handler %r failed to handle ZMQ message {parts=%s}: %s",
                       *this,
                       handler->id(),
                       parts,
                       std::current_exception());
        }
    }

}  // namespace core::zmq
