// -*- c++ -*-
//==============================================================================
/// @file python-pickle.h++
/// @brief Python pickle/unpickle environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "python-containerobject.h++"

namespace shared::python
{
    ContainerObject unpickle(const types::ByteVector &bytes);
    types::ByteVector pickle(const SimpleObject &object);
};  // namespace shared::python
