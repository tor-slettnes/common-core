// -*- c++ -*-
//==============================================================================
/// @file upgrade-types.c++
/// @brief Upgrade service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-types.h++"

namespace platform::upgrade
{
    PackageInfo::PackageInfo(const PackageSource &source,
                             const fs::path &package_name,
                             const std::string &product_name,
                             const sysconfig::Version &release_version,
                             const std::string &release_description,
                             bool reboot_required,
                             bool applicable)
        : source(source),
          package_name(package_name),
          product_name(product_name),
          release_version(release_version),
          release_description(release_description),
          reboot_required(reboot_required),
          applicable(applicable)
    {
    }

    void PackageInfo::to_stream(std::ostream &stream) const
    {
        core::str::format(stream,
                          "{source=%s, package_name=%r, product_name=%r, "
                          "release_version=%s, release_description=%r, "
                          "reboot_required=%b, applicable=%b}",
                          this->source,
                          this->package_name,
                          this->product_name,
                          this->release_version,
                          this->release_description,
                          this->reboot_required,
                          this->applicable);
    }
}  // namespace platform::upgrade

namespace std
{
    std::ostream &operator<<(std::ostream &stream,
                             const platform::upgrade::PackageSource &source)
    {
        if (const platform::vfs::Path *vpath = std::get_if<platform::vfs::Path>(&source))
        {
            core::str::format(stream, "{vpath=%s}", *vpath);
        }
        else if (const platform::upgrade::URL *url = std::get_if<platform::upgrade::URL>(&source))
        {
            core::str::format(stream, "{url=%s}", *url);
        }
        else
        {
            stream << "{}";
        }

        return stream;
    }

}  // namespace std
