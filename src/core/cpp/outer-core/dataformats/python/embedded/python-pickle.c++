// -*- c++ -*-
//==============================================================================
/// @file python-pickle.c++
/// @brief Python pickle/unpickle environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "python-pickle.h++"
#include "python-runtime.h++"

namespace cc::python
{
    ContainerObject unpickle(const types::ByteVector &bytes)
    {
        return runtime->call(
            "pickle",
            "loads",
            {SimpleObject::pybytes_from_bytes(bytes)});
    }

    types::ByteVector pickle(const SimpleObject &object)
    {
        ContainerObject result = runtime->call(
            "pickle",
            "dumps",
            SimpleObject::Vector({object}));

        if (const auto &bytes = result.as_bytevector())
        {
            return bytes.value();
        }
        else
        {
            return {};
        }
    }

    // Pickler pickler;
};  // namespace cc::python
