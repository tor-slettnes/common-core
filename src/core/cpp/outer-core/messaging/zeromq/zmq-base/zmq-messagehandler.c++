/// -*- c++ -*-
//==============================================================================
/// @file zmq-messagehandler.c++
/// @brief Process received message publications
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-messagehandler.h++"

namespace core::zmq
{
    MessageHandler::MessageHandler(const std::string &id, const Filter &filter)
        : id_(id),
          filter_(filter)
    {
    }

    const std::string &MessageHandler::id() const noexcept
    {
        return this->id_;
    }

    const Filter &MessageHandler::filter() const noexcept
    {
        return this->filter_;
    }

    void MessageHandler::handle_raw(const core::types::ByteVector &rawdata)
    {
        this->handle({rawdata.begin() + this->filter().size(),
                      rawdata.end()});
    }

}  // namespace core::zmq
