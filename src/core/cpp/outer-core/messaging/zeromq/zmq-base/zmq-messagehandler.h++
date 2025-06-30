/// -*- c++ -*-
//==============================================================================
/// @file zmq-messagehandler.h++
/// @brief Process received message publications
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-filter.h++"
#include "zmq-endpoint.h++"  // for `log_scope`
#include "types/bytevector.h++"

namespace core::zmq
{
    class Subscriber;

    class MessageHandler : public std::enable_shared_from_this<MessageHandler>
    {
    public:
        using Identity = std::string;

    protected:
        MessageHandler(
            const std::string &id,
            const Filter &filter,
            const std::weak_ptr<Subscriber> &subscriber = {});

        ~MessageHandler();

    public:
        const Identity &id() const noexcept;
        const Filter &filter() const noexcept;

        virtual void initialize();
        virtual void deinitialize();

        // Subclasses should implement one of the following method two to
        // process incoming message publications.

        // Override `handle_raw()` to receive the raw ZMQ data,
        // including the subscription filter.
        virtual void handle_raw(const core::types::ByteVector &rawdata);

        // Override `handle()` to receive only the "payload" part,
        // following the subscription filter.
        virtual void handle(const core::types::ByteVector &data) {}

    private:
        const std::string id_;
        const Filter filter_;
        std::weak_ptr<Subscriber> subscriber_;
    };

}  // namespace core::zmq
