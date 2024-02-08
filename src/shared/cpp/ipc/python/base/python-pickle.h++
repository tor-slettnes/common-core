// -*- c++ -*-
//==============================================================================
/// @file python-pickle.h++
/// @brief Python pickle/unpickle environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "python-runtime.h++"

namespace cc::python
{
    class Pickler : public Runtime
    {
    public:
        Pickler();

        ContainerObject unpickle(const types::ByteVector &bytes);
        types::ByteVector pickle(const Object &object);
    };

    extern Pickler pickler;
};  // namespace cc::python
