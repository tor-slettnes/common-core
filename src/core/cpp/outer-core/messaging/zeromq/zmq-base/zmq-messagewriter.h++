/// -*- c++ -*-
//==============================================================================
/// @file zmq-messagewriter.h++
/// @brief Publish messages with optional topic/header
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-publisher.h++"

namespace core::zmq
{
    using Header = core::types::ByteVector;

    class MessageWriter
    {
    protected:
        MessageWriter(const std::shared_ptr<Publisher> &publisher,
                      const std::optional<Header> &header = {});

    public:
        virtual void initialize() {}
        virtual void deinitialize() {}

        const std::optional<Header> &header() const noexcept;
        void write(const types::ByteVector &bytes);

    protected:
        std::shared_ptr<Publisher> publisher() const;

    private:
        std::weak_ptr<Publisher> publisher_;
        std::optional<Header> header_;
    };

}  // namespace core::zmq
