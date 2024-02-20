/// -*- c++ -*-
//==============================================================================
/// @file http-utils.c++
/// @brief Misc. HTTP utility functions
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "http-utils.h++"
#include "string/misc.h++"

#include <curl/curl.h>

#include <regex>

namespace shared::http
{
    bool decompose_header(const std::string &text, Header *header)
    {
        static std::regex rx(
            "(\\w[^\\s:]*):\\s*"                      // (1) field name
            "([^\\r\\n]+"                             // (2) initial content line
            "(?:(?:\\r\\n|\\r|\\n)\\s+[^\\r\\n]+)*)"  // +(2) additional content lines
            "(?:\\r\\n|\\r|\\n)"                      // trailing newline
        );

        auto matchit = std::sregex_iterator(text.begin(), text.end(), rx);
        uint endpos = 0;
        for (std::sregex_iterator endit; matchit != endit; matchit++)
        {
            header->emplace(matchit->str(1), matchit->str(2));
            endpos = matchit->position(0) + matchit->length(0);
        }
        return str::strip(text.substr(endpos)).empty();
    }

    bool successful_response(ResponseCode code)
    {
        switch (code / 100)
        {
        case 1:  // 1xx - informational responses
        case 2:  // 2xx - successul responses
            return true;

        case 3:  // 3xx - redirection responses
        case 4:  // 4xx - client error responses
        case 5:  // 5xx - server side responses
            return false;

        default:
            return false;
        }
    }

    std::string join_urls(const std::string &base,
                          const std::string &rel)
    {
        static const std::regex rx("\\w+://.*");
        if (std::regex_match(rel, rx))
        {
            return rel;
        }
        else
        {
            std::stringstream ss;
            bool base_slash = str::endswith(base, "/");
            bool rel_slash = str::startswith(rel, "/");

            ss.write(base.data(), base.size() - ((base_slash && rel_slash) ? 1 : 0));
            ss.write(rel.data(), rel.size());
            return ss.str();
        }
    }

    std::string url_encode(const std::string &decoded)
    {
        const auto encoded_value = curl_easy_escape(
            nullptr,
            decoded.c_str(),
            static_cast<int>(decoded.length()));
        std::string result(encoded_value);
        curl_free(encoded_value);
        return result;
    }

    std::string url_decode(const std::string &encoded)
    {
        int output_length;

        const auto decoded_value = curl_easy_unescape(
            nullptr,
            encoded.c_str(),
            static_cast<int>(encoded.length()),
            &output_length);

        std::string result(decoded_value, output_length);
        curl_free(decoded_value);
        return result;
    }


    static platform::InitTask global_init(
        "curl_global_init",
        std::bind(::curl_global_init, CURL_GLOBAL_ALL));

    static platform::ExitTask global_cleanup(
        "curl_global_cleanup",
        ::curl_global_cleanup);

}  // namespace shared::http
