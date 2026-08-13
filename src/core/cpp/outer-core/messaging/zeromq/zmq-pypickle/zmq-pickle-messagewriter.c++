/// -*- c++ -*-
//==============================================================================
/// @file zmq-pickle-messagewriter.c++
/// @brief Message writer for Python Pickle payloads
/// @author Tor Slettnes
//==============================================================================

#include "zmq-pickle-messagewriter.h++"
#include "python-pickle.h++"

namespace cc::zmq
{
    PyPickleMessageWriter::PyPickleMessageWriter(
        const std::shared_ptr<Publisher> &publisher,
        const std::optional<core::types::ByteVector> &header)
        : MessageWriter(publisher, header)
    {
    }

    void PyPickleMessageWriter::write(const python::SimpleObject &object)
    {
        MessageWriter::write(python::pickle(object));
    }

    void PyPickleMessageWriter::write(const core::types::Value &value)
    {
        this->write(
            cc::python::SimpleObject(
                python::SimpleObject::pyobj_from_value(value)));
    }

}  // namespace cc::zmq
