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
                          {SimpleObject::pybytes_from_bytes(bytes)},
                          {});
    }

    types::ByteVector Pickler::pickle(const SimpleObject &object)
    {
        ContainerObject result = this->call("dumps",
                                            SimpleObject::Vector({object}),
                                            SimpleObject::Map({}));
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
