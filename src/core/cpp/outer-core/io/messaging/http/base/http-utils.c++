/// -*- c++ -*-
//==============================================================================
/// @file http-utils.c++
/// @brief Misc. HTTP utility functions
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "http-utils.h++"
#include "string/misc.h++"
#include "status/exceptions.h++"

#include <curl/curl.h>

#include <regex>

namespace core::http
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

    void _curl_url_get(CURLUcode *code,
                       CURLU *handle,
                       CURLUPart part,
                       std::string *value,
                       uint flags)
    {
        if ((*code == CURLUE_OK) && (value != nullptr))
        {
            char *cvalue = nullptr;
            *code = ::curl_url_get(handle, part, &cvalue, flags);
            if (cvalue)
            {
                value->assign(cvalue);
                curl_free(cvalue);
            }
        }
    }

    void split_url(const std::string &url,
                   std::string *scheme,
                   std::string *username,
                   std::string *password,
                   std::string *host,
                   uint *port,
                   std::string *path,
                   Query *query,
                   std::string *fragment)
    {
        uint flags = CURLU_URLDECODE;
        CURLU *handle = curl_url();
        CURLUcode rc = curl_url_set(handle, CURLUPART_URL, url.data(), flags);

        _curl_url_get(&rc, handle, CURLUPART_SCHEME, scheme, flags);
        _curl_url_get(&rc, handle, CURLUPART_USER, username, flags);
        _curl_url_get(&rc, handle, CURLUPART_PASSWORD, password, flags);
        _curl_url_get(&rc, handle, CURLUPART_HOST, host, flags);

        if (port != nullptr)
        {
            std::string portstring;
            _curl_url_get(&rc, handle, CURLUPART_PORT, &portstring, flags);
            *port = str::convert_to<uint>(portstring, 0);
        }

        _curl_url_get(&rc, handle, CURLUPART_PATH, path, flags);

        if (query != nullptr)
        {
            std::string stringquery;
            query->clear();
            _curl_url_get(&rc, handle, CURLUPART_QUERY, &stringquery, 0);
            for (const std::string &part : str::split(stringquery, "&"))
            {
                std::list<std::string> kv = str::split(part, "=", 1);
                if (kv.size() == 2)
                {
                    query->emplace_back(url_decode(kv.front()),
                                        url_decode(kv.back()));
                }
            }
        }

        _curl_url_get(&rc, handle, CURLUPART_FRAGMENT, fragment, flags);
        curl_url_cleanup(handle);

        if (rc != CURLUE_OK)
        {
            throw exception::FailedPrecondition(
                curl_url_strerror(rc),
                {{"url", url},
                 {"curl_code", rc}});
        }
    }

    std::string join_url(const std::optional<std::string> &scheme,
                         const std::optional<std::string> &username,
                         const std::optional<std::string> &password,
                         const std::optional<std::string> &host,
                         const std::optional<uint> port,
                         const std::optional<std::string> &path,
                         const std::optional<Query> &query,
                         const std::optional<std::string> &fragment)

    {
        uint flags = CURLU_URLENCODE;
        CURLU *handle = curl_url();
        CURLUcode rc = CURLUE_OK;

        if (rc == CURLUE_OK)
        {
            rc = curl_url_set(handle, CURLUPART_SCHEME, scheme.value_or("http").data(), flags);
        }

        if ((rc == CURLUE_OK) && username)
        {
            rc = curl_url_set(handle, CURLUPART_USER, username->data(), flags);
        }

        if ((rc == CURLUE_OK) && password)
        {
            rc = curl_url_set(handle, CURLUPART_PASSWORD, password->data(), flags);
        }

        if (rc == CURLUE_OK)
        {
            rc = curl_url_set(handle, CURLUPART_HOST, host.value_or("localhost").data(), flags);
        }

        if ((rc == CURLUE_OK) && port)
        {
            rc = curl_url_set(handle, CURLUPART_PORT, str::to_string(*port).data(), flags);
        }

        if ((rc == CURLUE_OK) && path)
        {
            rc = curl_url_set(handle, CURLUPART_PATH, path->data(), flags);
        }

        if ((rc == CURLUE_OK) && query)
        {
            std::stringstream ss;
            std::string delimiter;

            for (const auto &[tag, value] : *query)
            {
                ss << delimiter;
                if (tag)
                {
                    ss << url_encode(*tag) << "=";
                }
                ss << url_encode(value.to_string());
                delimiter = "&";
            }

            std::string querystring = ss.str();
            rc = curl_url_set(handle, CURLUPART_QUERY, querystring.data(), flags);
        }

        if ((rc == CURLUE_OK) && fragment)
        {
            rc = curl_url_set(handle, CURLUPART_FRAGMENT, fragment->data(), flags);
        }

        std::string url;
        _curl_url_get(&rc, handle, CURLUPART_URL, &url, 0);
        curl_url_cleanup(handle);

        if (rc != CURLUE_OK)
        {
            throw exception::FailedPrecondition(
                curl_url_strerror(rc),
                {{"curl_code", rc}});
        }
        return url;
    }

    std::string join_urls(const std::string &base,
                          const std::string &rel)
    {
        static const std::regex rx("\\w+://.*");
        if (std::regex_match(rel, rx))
        {
            // `rel` is already a full URL starting with schema.
            return rel;
        }
        else
        {
            std::stringstream ss;

            // Eliminate double `/` when joining `base/` and `/rel`.
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

}  // namespace core::http
