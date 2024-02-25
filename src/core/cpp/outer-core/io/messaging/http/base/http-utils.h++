/// -*- c++ -*-
//==============================================================================
/// @file http-utils.h++
/// @brief Misc. HTTP utility functions
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/init.h++"
#include <string>
#include <unordered_map>

namespace shared::http
{
    using URL = std::string;
    using ResponseCode = long;
    using Header = std::unordered_multimap<std::string, std::string>;

    bool decompose_header(const std::string &text, Header *header);
    bool successful_response(ResponseCode code);

    std::string join_urls(const std::string &base, const std::string &rel);
    std::string url_encode(const std::string &decoded);
    std::string url_decode(const std::string &encoded);

}  // namespace shared::http
