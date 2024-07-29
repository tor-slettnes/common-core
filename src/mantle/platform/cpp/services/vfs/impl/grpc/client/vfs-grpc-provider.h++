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
        void initialize() override;

        void set_use_cached(bool use_cached);
        bool get_use_cached() const;

    public:
        ContextMap getContexts() const override;

        ContextMap getOpenContexts() const override;

        ContextRef getContext(
            const std::string &name,
            bool required) const override;

        ContextRef openContext(
            const std::string &name,
            bool required) override;

        void closeContext(
            const ContextRef &cxt) override;

        VolumeStats volumeStats(
            const Path &vpath,
            const OperationFlags &flags) const override;

        FileStats fileStats(
            const Path &vpath,
            const OperationFlags &flags) const override;

        Directory getDirectory(
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
            const Paths &sources,
            const OperationFlags &flags) const override;

        void createFolder(
            const Path &vpath,
            const OperationFlags &flags) const override;

        ReaderRef readFile(
            const Path &vpath) const override;

        WriterRef writeFile(
            const Path &vpath) const override;

        core::types::KeyValueMap getAttributes(
            const Path &vpath) const override;

        void setAttributes(
            const Path &vpath,
            const core::types::KeyValueMap &attributes) const override;

        void clearAttributes(
            const Path &vpath) const override;

    private:
        std::shared_ptr<Client> client;
        bool use_cached;
    };
}  // namespace platform::vfs::grpc
