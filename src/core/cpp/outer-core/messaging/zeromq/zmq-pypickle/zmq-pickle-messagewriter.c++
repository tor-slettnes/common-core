/// -*- c++ -*-
//==============================================================================
/// @file zmq-pickle-messagewiter.c++
/// @brief Message writer for Python Pickle payloads
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-pickle-messagewriter.h++"
#include "python-pickle.h++"

namespace core::zmq
{
    PyPickleMessageWriter::PyPickleMessageWriter(
        const std::shared_ptr<Publisher> &publisher,
        const Filter &filter)
        : PyPickleMessageBase(publisher),
          MessageWriter(publisher,
                        filter.empty() ? this->default_filter() : filter)
    {
    }

    void PyPickleMessageWriter::write(const python::SimpleObject &object)
    {
        MessageWriter::write(python::pickle(object));
    }

    void PyPickleMessageWriter::write(const types::Value &value)
    {
        this->write(python::SimpleObject::pyobj_from_value(value));
    }

}  // namespace core::zmq
