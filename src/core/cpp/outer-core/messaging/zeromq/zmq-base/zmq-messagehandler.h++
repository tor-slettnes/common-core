/// -*- c++ -*-
//==============================================================================
/// @file zmq-messagehandler.h++
/// @brief Process received message publications
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-filter.h++"
#include "zmq-endpoint.h++"     // for `log_scope`
#include "types/bytevector.h++"

namespace core::zmq
{
    class MessageHandler
    {
    protected:
        MessageHandler(const std::string &id, const Filter &filter);

    public:
        const std::string &id() const noexcept;
        const Filter &filter() const noexcept;

        virtual void initialize() {}
        virtual void deinitialize() {}

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
    };

}  // namespace core::zmq
