/// -*- c++ -*-
//==============================================================================
/// @file zmq-pypickle-messagehandler.h++
/// @brief Handle received messages with a Python pickle payload
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-pickle-messagebase.h++"
#include "zmq-messagehandler.h++"
#include "zmq-subscriber.h++"
#include "python-containerobject.h++"

namespace core::zmq
{
    class PyPickleMessageHandler : public PyPickleMessageBase,
                                   public MessageHandler
    {
        using This = PyPickleMessageHandler;

    protected:
        PyPickleMessageHandler(const std::shared_ptr<Subscriber> &subscriber,
                               const Filter &filter = {});

    private:
        void handle(const types::ByteVector &bytes) override;

    protected:
        // Subclasses should either of the following two methods to handle
        // received messages; either as an unpickled Python object or further
        // decoded as a variant value.

        virtual void handle_message(const python::ContainerObject &object);
        virtual void handle_message(const types::Value &value) {}
    };

}  // namespace core::zmq
