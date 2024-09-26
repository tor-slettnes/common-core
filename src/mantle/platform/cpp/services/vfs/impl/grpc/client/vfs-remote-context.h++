// -*- c++ -*-
//==============================================================================
/// @file vfs-remote-context.h++
/// @brief VFS service - remote file context
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs.h++"

namespace platform::vfs
{
    class RemoteContext : public Context,
                          public core::types::enable_create_shared<RemoteContext>
    {
    public:
        RemoteContext(
            const std::weak_ptr<ProviderInterface> &provider);

    public:
        void add_ref() override;
        void del_ref() override;
        // fs::path localPath(const fs::path &relpath) const override;

    private:
        std::weak_ptr<ProviderInterface> provider;
    };

}  // namespace platform::vfs
