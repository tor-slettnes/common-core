// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-provider.h++
/// @brief Virtual File SysConfig service - gRPC client implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-base.h++"
#include "vfs-grpc-client.h++"
#include "vfs-remote-context.h++"

namespace vfs::grpc
{
    class ClientProvider : public ProviderInterface,
                           public std::enable_shared_from_this<ClientProvider>
    {
        using This = ClientProvider;
        using Super = ProviderInterface;

    public:
        ClientProvider(const std::shared_ptr<Client> &client,
                       bool use_cached = false);

        bool is_pertinent() const override;
        void initialize() override;

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

        VolumeInfo get_volume_info(
            const Path &vpath,
            const OperationFlags &flags) const override;

        FileInfo get_file_info(
            const Path &vpath,
            const OperationFlags &flags) const override;

        Directory get_directory(
            const Path &vpath,
            const OperationFlags &flags) const override;

        Directory locate(
            const Path &vpath,
            const core::types::PathList &filename_masks,
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
            const Paths &vpaths,
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
        void on_context(
            core::signal::MappingAction action,
            const std::string &key,
            const ::cc::platform::vfs::Signal &signal) const;

        void on_context_in_use(
            core::signal::MappingAction action,
            const std::string &key,
            const ::cc::platform::vfs::Signal &signal) const;

        ContextMap context_map(
            const ::cc::platform::vfs::ContextMap &msg) const;

        std::shared_ptr<RemoteContext> decoded_context(
            const ::cc::platform::vfs::ContextSpec &spec) const;

    private:
        std::shared_ptr<Client> client;
        bool use_cached;
    };
}  // namespace vfs::grpc
