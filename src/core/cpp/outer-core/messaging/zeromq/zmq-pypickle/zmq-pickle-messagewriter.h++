/// -*- c++ -*-
//==============================================================================
/// @file zmq-pickle-messagewriter.h++
/// @brief Message writer for Python Pickle payloads
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "zmq-messagewriter.h++"
#include "python-simpleobject.h++"

namespace cc::zmq
{
    class PyPickleMessageWriter : public MessageWriter
    {
    protected:
        PyPickleMessageWriter(
            const std::shared_ptr<Publisher> &publisher,
            const std::optional<core::types::ByteVector> &header = {});

        void write(const python::SimpleObject &object);
        void write(const core::types::Value &value);
    };

}  // namespace cc::zmq
