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
        this->keep_receiving = false;
        if (std::thread t = std::move(this->receive_thread); t.joinable())
        {
            t.detach();
        }
    }

    void Subscriber::initialize()
    {
        Super::initialize();
        this->start_receiving();

        // Workaround for apparent `libzmq3` bug:
        // Setting a non-empty filter (see `add_handler_filter()` below) seems
        // to have no effect.  Instead, we must add an empty filter if we want
        // to receive messages at all.
        this->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    }

    void Subscriber::deinitialize()
    {
        // Workaround for apparent `libzmq3` bug:
        // Remove our empty subscription filter.
        this->setsockopt(ZMQ_UNSUBSCRIBE, "", 0);

        //this->clear();
        this->stop_receiving();

        Super::deinitialize();
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
                handler->deinitialize();
            }
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
    }

    void Subscriber::receive_loop()
    {
        logf_debug("%s listening for publications from %s",
                   *this,
                   this->address());

        try
        {
            while (this->keep_receiving)
            {
                if (auto bytes = this->receive())
                {
                    this->process_message(*bytes);
                }
            }

            logf_debug("%s is no longer listening for publications from %s",
                       *this,
                       this->address());
        }
        catch (const Error &e)
        {
            this->log_zmq_error("could not continue receiving publications", e);
            this->keep_receiving = false;
        }
    }

    void Subscriber::process_message(const types::ByteVector &bytes)
    {
        std::scoped_lock lock(this->mtx_);

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

    void Subscriber::add_handler_filter(const std::shared_ptr<MessageHandler> &handler)
    {
        const Filter &filter = handler->filter();
        logf_debug("%s adding subscription for %r with filter %r (size %d)",
                   *this,
                   handler->id(),
                   filter,
                   filter.size());
        this->setsockopt(ZMQ_SUBSCRIBE, filter.data(), filter.size());
        // this->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    }

    void Subscriber::remove_handler_filter(const std::shared_ptr<MessageHandler> &handler)
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
