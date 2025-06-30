/// -*- c++ -*-
//==============================================================================
/// @file zmq-messagehandler.c++
/// @brief Process received message publications
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-messagehandler.h++"
#include "zmq-subscriber.h++"

namespace core::zmq
{
    MessageHandler::MessageHandler(
        const std::string &id,
        const Filter &filter,
        const std::weak_ptr<Subscriber> &subscriber)
        : id_(id),
          filter_(filter),
          subscriber_(subscriber)
    {
    }

    MessageHandler::~MessageHandler()
    {
        try
        {
            if (std::shared_ptr<Subscriber> subscriber = this->subscriber_.lock())
            {
                subscriber->remove_handler(this->shared_from_this());
            }
        }
        catch (std::bad_weak_ptr)
        {
        }
    }

    const std::string &MessageHandler::id() const noexcept
    {
        return this->id_;
    }

    const Filter &MessageHandler::filter() const noexcept
    {
        return this->filter_;
    }

    void MessageHandler::initialize()
    {
        if (std::shared_ptr<Subscriber> subscriber = this->subscriber_.lock())
        {
            subscriber->add_handler(this->shared_from_this());
        }
    }

    void MessageHandler::deinitialize()
    {
        if (std::shared_ptr<Subscriber> subscriber = this->subscriber_.lock())
        {
            subscriber->remove_handler(this->shared_from_this());
        }
    }

    void MessageHandler::handle_raw(const core::types::ByteVector &rawdata)
    {
        this->handle({rawdata.begin() + this->filter().size(),
                      rawdata.end()});
    }

}  // namespace core::zmq
