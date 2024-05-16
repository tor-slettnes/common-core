/// -*- c++ -*-
//==============================================================================
/// @file zmq-pypickle-messagehandler.h++
/// @brief Handle received messages with a Python pickle payload
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-pickle-messagehandler.h++"
#include "python-pickle.h++"
#include "platform/symbols.h++"

namespace core::zmq
{
    PyPickleMessageHandler::PyPickleMessageHandler(
        const std::shared_ptr<Subscriber> &subscriber,
        const Filter &filter)
        : PyPickleMessageBase(subscriber),
          MessageHandler(TYPE_NAME_BASE(This),
                         filter.empty() ? this->default_filter() : filter)
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
