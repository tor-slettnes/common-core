/// -*- c++ -*-
//==============================================================================
/// @file reader.h++
/// @brief YAML generic parser instance
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "libyaml/yaml-reader.h++"

namespace core::yaml
{
    using Reader = YamlReader;
    extern Reader reader;
}  // namespace core::yaml
