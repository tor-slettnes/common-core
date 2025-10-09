/// -*- c++ -*-
//==============================================================================
/// @file zmq-pickle-messagehandler.c++
/// @brief Handle received messages with a Python pickle payload
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-pickle-messagehandler.h++"
#include "python-pickle.h++"
#include "platform/symbols.h++"

namespace core::zmq
{
    PyPickleMessageHandler::PyPickleMessageHandler(
        const std::weak_ptr<Subscriber> &subscriber,
        const std::optional<Filter> &filter)
        : MessageHandler(TYPE_NAME_BASE(This), filter, subscriber)
    {
    }

    void PyPickleMessageHandler::handle(const types::ByteVector &bytes)
    {
        this->handle_message(python::unpickle(bytes));
    }

    void PyPickleMessageHandler::handle_message(const python::ContainerObject &object)
    {
        this->handle_message(object.as_value());
    }

}  // namespace core::zmq
