// -*- c++ -*-
//==============================================================================
/// @file vfs-provider.c++
/// @brief VFS service - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-base.h++"

namespace platform::vfs
{
    //==========================================================================
    // Provider

    Location ProviderInterface::location(
        const Path &vpath,
        bool modify) const
    {
        return this->location(this->get_context(vpath.context), vpath.relpath, modify);
    }

    Location ProviderInterface::location(
        const ContextName &context,
        const fs::path &relpath,
        bool modify) const
    {
        return this->location(this->get_context(context), relpath, modify);
    }

    Location ProviderInterface::location(
        const Context::ptr &cxt,
        const fs::path &relpath,
        bool modify) const
    {
        return Location(cxt, relpath, modify);
    }

    LocationList ProviderInterface::locations(const Paths &vpaths,
                                              bool modify) const
    {
        LocationList locations;
        locations.reserve(vpaths.size());
        for (const auto &vpath : vpaths)
        {
            locations.emplace_back(this->location(vpath, modify));
        }
        return locations;
    }

    ContextProxy ProviderInterface::context_proxy(
        const std::string &name,
        bool modify) const
    {
        return this->context_proxy(this->get_context(name), modify);
    }

    ContextProxy ProviderInterface::context_proxy(
        const Context::ptr &cxt,
        bool modify) const
    {
        return ContextProxy(cxt, modify);
    }

    //==========================================================================
    // Provider instance

    core::platform::ProviderProxy<ProviderInterface> vfs("VFS");
}  // namespace platform::vfs
