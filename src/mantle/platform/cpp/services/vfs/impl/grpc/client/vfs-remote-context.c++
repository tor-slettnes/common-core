// -*- c++ -*-
//==============================================================================
/// @file vfs-remote-context.c++
/// @brief VFS service - remote file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-remote-context.h++"
#include "vfs-grpc-client.h++"

namespace platform::vfs
{
    RemoteContext::RemoteContext(
        const std::weak_ptr<ProviderInterface> &provider)
        : Context(),
          provider(provider)
    {
    }

    void RemoteContext::add_ref()
    {
        if (auto provider = this->provider.lock())
        {
            provider->open_context(this->name);
        }
    }

    void RemoteContext::del_ref()
    {
        if (auto provider = this->provider.lock())
        {
            provider->close_context(this->name);
        }
    }
}  // namespace platform::vfs::grpc
