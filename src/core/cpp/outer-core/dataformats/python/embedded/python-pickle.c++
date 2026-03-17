// -*- c++ -*-
//==============================================================================
/// @file python-pickle.c++
/// @brief Python pickle/unpickle environment
/// @author Tor Slettnes
//==============================================================================

#include "python-pickle.h++"
#include "python-runtime.h++"

namespace core::python
{
    ContainerObject unpickle(const types::ByteVector &bytes)
    {
        return core::python::runtime->call(
            "pickle",
            "loads",
            SimpleObject::Vector({SimpleObject::pybytes_from_bytes(bytes)}));
    }

    types::ByteVector pickle(const SimpleObject &object)
    {
        ContainerObject result = core::python::runtime->call(
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
};  // namespace core::python
