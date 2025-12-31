// -*- c++ -*-
//==============================================================================
/// @file python-pickle.h++
/// @brief Python pickle/unpickle environment
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "python-containerobject.h++"

namespace core::python
{
    ContainerObject unpickle(const types::ByteVector &bytes);
    types::ByteVector pickle(const SimpleObject &object);
};  // namespace core::python
