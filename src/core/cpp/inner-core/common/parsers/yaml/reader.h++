/// -*- c++ -*-
//==============================================================================
/// @file reader.h++
/// @brief YAML generic parser instance
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "libyaml/yaml-reader.h++"

namespace core::yaml
{
    using Reader = YamlReader;
    extern Reader reader;
}  // namespace core::yaml
