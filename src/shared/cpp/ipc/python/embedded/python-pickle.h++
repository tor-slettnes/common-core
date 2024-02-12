// -*- c++ -*-
//==============================================================================
/// @file python-pickle.h++
/// @brief Python pickle/unpickle environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "python-containerobject.h++"
#include "python-runtime.h++"

namespace shared::python
{
    class Pickler : public Runtime
    {
    public:
        Pickler();

        ContainerObject unpickle(const types::ByteVector &bytes);
        types::ByteVector pickle(const SimpleObject &object);
    };

    extern Pickler pickler;
};  // namespace shared::python
