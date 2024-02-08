// -*- c++ -*-
//==============================================================================
/// @file python-pickle.c++
/// @brief Python pickle/unpickle environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "python-pickle.h++"

namespace cc::python
{
    Pickler::Pickler()
        : Runtime("pickle")
    {
    }

    ContainerObject Pickler::unpickle(const types::ByteVector &bytes)
    {
        return this->call("loads",
                          {Object::pybytes_from_bytes(bytes)},
                          {});
    }

    types::ByteVector Pickler::pickle(const Object &object)
    {
        Object result = this->call("dumps",
                                   Object::Vector({object}),
                                   Object::Map({}));
        if (const auto &bytes = result.as_bytevector())
        {
            return bytes.value();
        }
        else
        {
            return {};
        }
    }

    Pickler pickler;
};  // namespace cc::python
