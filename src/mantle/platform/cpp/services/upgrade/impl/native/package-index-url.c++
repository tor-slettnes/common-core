// -*- c++ -*-
//==============================================================================
/// @file package-index-url.c++
/// @brief Upgrade from local package
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-index-url.h++"
#include "package-info.h++"
#include "http-client.h++"
#include "parsers/json/reader.h++"

namespace upgrade::native
{
    URLPackageIndex::URLPackageIndex(const URL &url)
        : url(url)
    {
    }

    PackageSource URLPackageIndex::package_source(const std::string &package_name) const
    {
        std::string base_url = this->url.substr(0, this->url.rfind("/")) + "/";
        return {core::http::join_urls(base_url, package_name)};
    }

    PackageCatalogue URLPackageIndex::scan()
    {
        PackageCatalogue packages;

        logf_debug("Package scan at URL: %s", this->url);

        core::http::HTTPClient http(this->url);
        std::stringstream ss = http.get({});
        core::types::Value response = core::json::reader.read_stream(ss);

        for (const auto &[package_name, package_info_data] : response.get_kvmap())
        {
            if (core::types::KeyValueMapPtr package_info_settings = package_info_data.get_kvmap_ptr())
            {
                auto package_info = std::make_shared<NativePackageInfo>(
                    *package_info_settings,
                    this->package_source(package_name));
                packages.push_back(package_info);
                logf_debug("Added upgrade package: %s", package_info->source());
            }
        }

        this->available_packages = packages;
        return packages;
    }

}  // namespace upgrade::native
