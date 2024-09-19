// -*- c++ -*-
//==============================================================================
/// @file package-index-url.c++
/// @brief Upgrade from local package
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-index-url.h++"
#include "package-manifest.h++"
#include "http-client.h++"
#include "json/reader.h++"

namespace platform::upgrade::native
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

    PackageManifests URLPackageIndex::scan()
    {
        PackageManifests manifests;

        logf_debug("Package scan at URL: %s", this->url);

        core::http::HTTPClient http(this->url);
        std::stringstream ss = http.get({});
        core::types::Value response = core::json::reader.read_stream(ss);

        if (core::types::KeyValueMapPtr package_map = response.get_kvmap())
        {
            for (const auto &[package_name, manifest_data] : *package_map)
            {
                if (core::types::KeyValueMapPtr manifest_settings = manifest_data.get_kvmap())
                {
                    auto manifest = std::make_shared<LocalManifest>(
                        *manifest_settings,
                        this->package_source(package_name));
                    manifests.push_back(manifest);
                    logf_debug("Added upgrade package: %s", manifest->source());
                }
            }
        }

        this->available_packages = manifests;
        return manifests;
    }

}  // namespace platform::upgrade::native
