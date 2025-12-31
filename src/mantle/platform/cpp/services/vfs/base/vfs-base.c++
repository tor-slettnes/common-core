// -*- c++ -*-
//==============================================================================
/// @file vfs-base.c++
/// @brief VFS service - abstract interface
/// @author Tor Slettnes
//==============================================================================

#include "vfs-base.h++"

namespace vfs
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

    std::optional<std::string> ProviderInterface::read_chunk(
        std::istream &stream)
    {
        stream.peek();  // Trigger underflow() if buffer is empty
        if (stream.good())
        {
            std::streamsize chunksize = stream.rdbuf()->in_avail();
            std::string buf(chunksize, '\0');
            stream.readsome(buf.data(), buf.size());
            return buf;
        }
        else
        {
            return {};
        }
    }

    std::streamsize ProviderInterface::write_chunk(
        std::ostream &stream,
        const std::string &chunk)
    {
        stream.write(chunk.data(), chunk.size());
        if (stream.good())
        {
            return chunk.size();
        }
        else
        {
            return 0;
        }
    }

    //==========================================================================
    // Provider instance

    core::platform::ProviderProxy<ProviderInterface> vfs("VFS");
}  // namespace vfs
