/// -*- c++ -*-
//==============================================================================
/// @file reader.h++
/// @brief YAML generic parser instance
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "libyaml/yaml-reader.h++"

namespace cc::core::yaml
{
    using Reader = YamlReader;
    extern Reader reader;
}  // namespace cc::core::yaml
