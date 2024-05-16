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

        // Subclasses should implement the following method to process
        // incoming message publications.

        virtual void handle(const types::ByteVector &data) = 0;

    private:
        const std::string id_;
        const Filter filter_;
    };

}  // namespace core::zmq
