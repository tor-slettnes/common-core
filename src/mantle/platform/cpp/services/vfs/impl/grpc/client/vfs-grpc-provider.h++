// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-provider.h++
/// @brief Virtual File SysConfig service - gRPC client implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-base.h++"
#include "vfs-grpc-client.h++"

namespace platform::vfs::grpc
{
    class ClientProvider : public ProviderInterface
    {
        using This = ClientProvider;
        using Super = ProviderInterface;

    public:
        ClientProvider(const std::shared_ptr<Client> &client,
                       bool use_cached = false);

        bool is_pertinent() override;

        void set_use_cached(bool use_cached);
        bool get_use_cached() const;

    public:
        ContextMap get_contexts() const override;

        ContextMap get_open_contexts() const override;

        Context::ptr get_context(
            const std::string &name,
            bool required) const override;

        Context::ptr open_context(
            const std::string &name,
            bool required) override;

        void close_context(
            const std::string &name,
            bool required) override;

        void close_context(
            const Context::ptr &cxt) override;

        VolumeStats volume_stats(
            const Path &vpath,
            const OperationFlags &flags) const override;

        FileStats file_stats(
            const Path &vpath,
            const OperationFlags &flags) const override;

        Directory get_directory(
            const Path &vpath,
            const OperationFlags &flags) const override;

        Directory locate(
            const Path &vpath,
            const std::vector<std::string> &filename_masks,
            const core::types::TaggedValueList &attribute_filters,
            const OperationFlags &flags) const override;

        void copy(
            const Paths &sources,
            const Path &target,
            const OperationFlags &flags) const override;

        void move(
            const Paths &sources,
            const Path &target,
            const OperationFlags &flags) const override;

        void remove(
            const Path &vpath,
            const OperationFlags &flags) const override;

        void create_folder(
            const Path &vpath,
            const OperationFlags &flags) const override;

        UniqueReader read_file(
            const Path &vpath) const override;

        UniqueWriter write_file(
            const Path &vpath) const override;

        core::types::KeyValueMap get_attributes(
            const Path &vpath) const override;

        void set_attributes(
            const Path &vpath,
            const core::types::KeyValueMap &attributes) const override;

        void clear_attributes(
            const Path &vpath) const override;

    private:
        ContextMap context_map(
            const ::cc::platform::vfs::ContextMap &msg) const;

    private:
        std::shared_ptr<Client> client;
        bool use_cached;
    };
}  // namespace platform::vfs::grpc
