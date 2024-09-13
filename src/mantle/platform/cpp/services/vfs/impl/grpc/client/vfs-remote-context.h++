// -*- c++ -*-
//==============================================================================
/// @file vfs-remote-context.h++
/// @brief VFS service - remote file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs.h++"
#include "vfs-grpc-client.h++"

namespace platform::vfs::grpc
{
    class RemoteContext : public Context,
                          public core::types::enable_create_shared<RemoteContext>
    {
    protected:
        RemoteContext(
            const std::shared_ptr<platform::vfs::grpc::Client> &client);

    public:
        using Context::Context;

        void add_ref() override;
        void del_ref() override;
        // fs::path localPath(const fs::path &relpath) const override;

    private:
        std::shared_ptr<platform::vfs::grpc::Client> client;
    };

}  // namespace platform::vfs
