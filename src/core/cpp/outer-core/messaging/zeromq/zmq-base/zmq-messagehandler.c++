/// -*- c++ -*-
//==============================================================================
/// @file zmq-messagehandler.c++
/// @brief Process received message publications
/// @author Tor Slettnes
//==============================================================================

#include "zmq-messagehandler.h++"
#include "zmq-subscriber.h++"

namespace core::zmq
{
    MessageHandler::MessageHandler(
        const std::string &id,
        const std::optional<Filter> &filter,
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

    const std::optional<Filter> &MessageHandler::filter() const noexcept
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

    void MessageHandler::handle(const MessageParts &parts)
    {
        this->handle(this->combine_parts(parts, this->filter().has_value()));
    }

    core::types::ByteVector MessageHandler::combine_parts(
        const MessageParts &parts,
        bool remove_header) const
    {
        core::types::ByteVector payload;
        if (!parts.empty())
        {
            for (auto it = parts.begin() + remove_header; it != parts.end(); it++)
            {
                payload.insert(payload.end(), it->begin(), it->end());
            }
        }
        return payload;
    }

}  // namespace core::zmq
