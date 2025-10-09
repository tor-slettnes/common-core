/// -*- c++ -*-
//==============================================================================
/// @file zmq-pickle-messagewriter.h++
/// @brief Message writer for Python Pickle payloads
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-messagewriter.h++"
#include "python-simpleobject.h++"

namespace core::zmq
{
    class PyPickleMessageWriter : public MessageWriter
    {
    protected:
        PyPickleMessageWriter(
            const std::shared_ptr<Publisher> &publisher,
            const std::optional<core::types::ByteVector> &header = {});

        void write(const python::SimpleObject &object);
        void write(const types::Value &value);
    };

}  // namespace core::zmq
