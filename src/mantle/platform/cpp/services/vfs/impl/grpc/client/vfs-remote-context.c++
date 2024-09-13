// -*- c++ -*-
//==============================================================================
/// @file vfs-remote-context.c++
/// @brief VFS service - remote file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-remote-context.h++"
#include "vfs.h++"

namespace platform::vfs::grpc
{
    RemoteContext::RemoteContext(
        const std::shared_ptr<platform::vfs::grpc::Client> &client)
        : Context(),
          client(client)
    {
    }

    void RemoteContext::add_ref()
    {
        ::cc::platform::vfs::Path request;
        request.set_context(this->name);
        this->client->call_check(&Client::Stub::open_context, request);
    }

    void RemoteContext::del_ref()
    {
        ::cc::platform::vfs::Path request;
        request.set_context(this->name);
        this->client->call_check(&Client::Stub::close_context, request);
    }
}  // namespace platform::vfs
