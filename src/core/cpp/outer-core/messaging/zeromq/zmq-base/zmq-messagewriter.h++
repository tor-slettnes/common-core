/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-messagewriter.h++
/// @brief Publish messages with optional topic filter
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-publisher.h++"
#include "zmq-filter.h++"

namespace core::zmq
{
    class MessageWriter
    {
    protected:
        MessageWriter(const std::shared_ptr<Publisher> &publisher,
                      const Filter &filter = {});

    public:
        virtual void initialize() {}
        virtual void deinitialize() {}

        const Filter &filter() const noexcept;
        void write(const types::ByteVector &bytes);

    protected:
        std::shared_ptr<Publisher> publisher() const;

    private:
        std::weak_ptr<Publisher> publisher_;
        Filter filter_;
    };

}  // namespace core::zmq
