// -*- c++ -*-
//==============================================================================
/// @file python-pickle.c++
/// @brief Python pickle/unpickle environment
/// @author Tor Slettnes
//==============================================================================

#include "python-pickle.h++"
#include "python-runtime.h++"

namespace cc::python
{
    ContainerObject unpickle(const core::types::ByteVector& bytes)
    {
        return cc::python::runtime->call(
            "pickle",
            "loads",
            SimpleObject::Vector({SimpleObject::pybytes_from_bytes(bytes)}));
    }

    core::types::ByteVector pickle(const SimpleObject& object)
    {
        ContainerObject result = cc::python::runtime->call(
            "pickle",
            "dumps",
            SimpleObject::Vector({object}));

        if (const auto& bytes = result.as_bytevector())
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
