/// -*- c++ -*-
//==============================================================================
/// @file http-utils.h++
/// @brief Misc. HTTP utility functions
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/value.h++"
#include <string>
#include <unordered_map>

namespace core::http
{
    using URL = std::string;
    using ResponseCode = long;
    using Header = std::unordered_multimap<std::string, std::string>;
    using Query = types::TaggedValueList;

    bool decompose_header(const std::string &text, Header *header);
    bool successful_response(ResponseCode code);

    std::string join_path_query(const std::string &path,
                                const Query &query);

    void split_url(const std::string &url,
                   std::string *scheme,
                   std::string *username,
                   std::string *password,
                   std::string *host,
                   uint *port,
                   std::string *path,
                   Query *query,
                   std::string *fragment);

    std::string join_url(const std::optional<std::string> &scheme,
                         const std::optional<std::string> &username,
                         const std::optional<std::string> &password,
                         const std::optional<std::string> &host,
                         const std::optional<uint> port,
                         const std::optional<std::string> &path,
                         const std::optional<Query> &query,
                         const std::optional<std::string> &fragment);

    std::string join_urls(const std::string &base, const std::string &rel);
    std::string url_encode(const std::string &decoded);
    std::string url_decode(const std::string &encoded);
}  // namespace core::http
