// -*- c++ -*-
//==============================================================================
/// @file python-pickle.h++
/// @brief Python pickle/unpickle environment
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "python-containerobject.h++"

namespace cc::python
{
    ContainerObject unpickle(const core::types::ByteVector &bytes);
    core::types::ByteVector pickle(const SimpleObject &object);
};  // namespace cc::python
