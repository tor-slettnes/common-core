/// -*- c++ -*-
//==============================================================================
/// @file zmq-pickle-messagewiter.h++
/// @brief Message writer for Python Pickle payloads
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-pickle-messagebase.h++"
#include "zmq-messagewriter.h++"
#include "python-simpleobject.h++"

namespace core::zmq
{
    class PyPickleMessageWriter : public PyPickleMessageBase,
                                  public MessageWriter
    {
    protected:
        PyPickleMessageWriter(const std::shared_ptr<Publisher> &publisher,
                              const Filter &filter = {});

        void write(const python::SimpleObject &object);
        void write(const types::Value &value);
    };

}  // namespace core::zmq
