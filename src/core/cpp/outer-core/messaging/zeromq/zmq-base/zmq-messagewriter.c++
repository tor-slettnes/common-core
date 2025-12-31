/// -*- c++ -*-
//==============================================================================
/// @file zmq-messagewriter.c++
/// @brief Publish messages with optional topic filter
/// @author Tor Slettnes
//==============================================================================

#include "zmq-messagewriter.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

namespace core::zmq
{
    MessageWriter::MessageWriter(const std::shared_ptr<Publisher> &publisher,
                                 const std::optional<Header> &header)
        : publisher_(publisher),
          header_(header)
    {
        log_trace("Created ZMQ writer with header: ", header);
    }

    const std::optional<Header> &MessageWriter::header() const noexcept
    {
        return this->header_;
    }

    std::shared_ptr<Publisher> MessageWriter::publisher() const
    {
        return this->publisher_.lock();
    }

    void MessageWriter::write(const types::ByteVector &bytes)
    {
        if (auto publisher = this->publisher())
        {
            if (publisher->initialized())
            {
                publisher->publish(this->header(), bytes);
            }
        }
    }

}  // namespace core::zmq
