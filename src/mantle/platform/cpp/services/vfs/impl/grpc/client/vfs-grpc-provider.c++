// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-provider.c++
/// @brief Virtual File System service - gRPC client implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-grpc-provider.h++"
#include "vfs-grpc-stream.h++"
#include "vfs-remote-context.h++"
#include "protobuf-vfs.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

namespace platform::vfs::grpc
{
    const vfs::Path nullpath;

    ClientProvider::ClientProvider(const std::shared_ptr<Client> &client,
                                   bool use_cached)
        : Super("vfs::ClientProvider"),
          client(client),
          use_cached(use_cached)
    {
    }

    bool ClientProvider::is_pertinent()
    {
        return bool(client);
    }

    void ClientProvider::initialize()
    {
        Provider::initialize();
        using namespace std::placeholders;

        this->client->add_mapping_handler(
            cc::platform::vfs::Signal::kContext,
            [](core::signal::MappingAction action,
               const std::string &key,
               const cc::platform::vfs::Signal &signal) {
                platform::vfs::signal_context.emit(
                    action,
                    key,
                    protobuf::decode_shared<RemoteContext>(signal.context()));
            });

        this->client->add_mapping_handler(
            cc::platform::vfs::Signal::kContextInUse,
            [](core::signal::MappingAction action,
               const std::string &key,
               const cc::platform::vfs::Signal &signal) {
                platform::vfs::signal_context_in_use.emit(
                    action,
                    key,
                    protobuf::decode_shared<RemoteContext>(signal.context_in_use()));
            });
    }

    void ClientProvider::set_use_cached(bool use_cached)
    {
        this->use_cached = use_cached;
    }

    bool ClientProvider::get_use_cached() const
    {
        return this->use_cached;
    }

    ContextMap ClientProvider::getContexts() const
    {
        if (this->get_use_cached())
        {
            return platform::vfs::signal_context.get_cached();
        }
        else
        {
            return protobuf::decoded<ContextMap>(
                client->call_check("vfs::getContexts",
                                   &Client::Stub::getContexts));
        }
    }

    ContextMap ClientProvider::getOpenContexts() const
    {
        if (this->get_use_cached())
        {
            return platform::vfs::signal_context_in_use.get_cached();
        }
        else
        {
            return protobuf::decoded<ContextMap>(
                client->call_check("vfs::getOpenContexts",
                                   &Client::Stub::getOpenContexts));
        }
    }

    ContextRef ClientProvider::getContext(const std::string &name, bool required) const
    {
        ContextRef cxt;
        if (this->get_use_cached())
        {
            cxt = signal_context.get_cached(name, {});
            if (required && !cxt)
            {
                throwf(::core::exception::NotFound,
                       ("No such filesystem context: %r", name),
                       name);
            }
        }
        else
        {
            cc::platform::vfs::Path request;
            request.set_context(name);

            try
            {
                cxt = protobuf::decode_shared<RemoteContext>(
                    client->call_check("vfs::getContextSpec",
                                       &Client::Stub::getContextSpec,
                                       request));
            }
            catch (...)
            {
                if (required)
                {
                    throw;
                }
            }
        }
        return cxt;
    }

    ContextRef ClientProvider::openContext(const std::string &name, bool required)
    {
        cc::platform::vfs::Path request;
        request.set_context(name);
        try
        {
            return protobuf::decode_shared<RemoteContext>(
                client->call_check("vfs::openContext",
                                   &Client::Stub::openContext,
                                   request));
        }
        catch (...)
        {
            if (required)
            {
                throw;
            }
            else
            {
                return {};
            }
        }
    }

    void ClientProvider::closeContext(const ContextRef &cxt)
    {
        if (cxt)
        {
            cc::platform::vfs::Path request;
            request.set_context(cxt->name);

            try
            {
                client->call_check("vfs::closeContext",
                                   &Client::Stub::closeContext,
                                   request);
            }
            catch (const std::exception &e)
            {
                logf_notice("Failed to close context %r: %r", cxt->name, e);
            }
        }
    }

    VolumeStats ClientProvider::volumeStats(const Path &vpath,
                                            const OperationFlags &flags) const
    {
        return protobuf::decoded<VolumeStats>(
            client->call_check(
                "vfs::volumeStats",
                &Client::Stub::volumeStats,
                protobuf::encoded<cc::platform::vfs::PathRequest>(vpath, nullpath, flags)));
    }

    FileStats ClientProvider::fileStats(const Path &vpath,
                                        const OperationFlags &flags) const
    {
        return protobuf::decoded<FileStats>(
            client->call_check(
                "vfs::fileStats",
                &Client::Stub::fileStats,
                protobuf::encoded<cc::platform::vfs::PathRequest>(vpath, nullpath, flags)));
    }

    Directory ClientProvider::getDirectory(const Path &vpath,
                                           const OperationFlags &flags) const
    {
        return protobuf::decoded<Directory>(
            client->call_check(
                "vfs::getDirectory",
                &Client::Stub::getDirectory,
                protobuf::encoded<cc::platform::vfs::PathRequest>(vpath, nullpath, flags)));
    }

    Directory ClientProvider::locate(const Path &root,
                                     const std::vector<std::string> &filename_masks,
                                     const core::types::TaggedValueList &attribute_filters,
                                     const OperationFlags &flags) const
    {
        return protobuf::decoded<Directory>(
            client->call_check(
                "vfs::locate",
                &Client::Stub::locate,
                protobuf::encoded<cc::platform::vfs::LocateRequest>(
                    root, filename_masks, attribute_filters, flags)));
    }

    void ClientProvider::copy(const Paths &sources,
                              const Path &target,
                              const OperationFlags &flags) const
    {
        client->call_check(
            "vfs::copy",
            &Client::Stub::copy,
            protobuf::encoded<cc::platform::vfs::PathRequest>(sources, target, flags));
    }

    void ClientProvider::move(const Paths &sources,
                              const Path &target,
                              const OperationFlags &flags) const
    {
        client->call_check(
            "vfs::move",
            &Client::Stub::move,
            protobuf::encoded<cc::platform::vfs::PathRequest>(sources, target, flags));
    }

    void ClientProvider::remove(const Paths &sources,
                                const OperationFlags &flags) const
    {
        client->call_check(
            "vfs::remove",
            &Client::Stub::remove,
            protobuf::encoded<cc::platform::vfs::PathRequest>(sources, nullpath, flags));
    }

    void ClientProvider::createFolder(const Path &vpath,
                                      const OperationFlags &flags) const
    {
        client->call_check(
            "vfs::createFolder",
            &Client::Stub::createFolder,
            protobuf::encoded<cc::platform::vfs::PathRequest>(nullpath, vpath, flags));
    }

    ReaderRef ClientProvider::readFile(const Path &vpath) const
    {
        return std::make_unique<ClientInputStream>(client->stub, vpath);
    }

    WriterRef ClientProvider::writeFile(const Path &vpath) const
    {
        return std::make_unique<ClientOutputStream>(client->stub, vpath);
    }

    core::types::KeyValueMap ClientProvider::getAttributes(const Path &vpath) const
    {
        return protobuf::decoded<core::types::KeyValueMap>(
            client->call_check(
                "vfs::getAttributes",
                &Client::Stub::getAttributes,
                protobuf::encoded<cc::platform::vfs::Path>(vpath)));
    }

    void ClientProvider::setAttributes(const Path &vpath,
                                       const core::types::KeyValueMap &attributes) const
    {
        client->call_check(
            "vfs::setAttributes",
            &Client::Stub::setAttributes,
            protobuf::encoded<cc::platform::vfs::AttributeRequest>(vpath, attributes));
    }

    void ClientProvider::clearAttributes(const Path &vpath) const
    {
        client->call_check(
            "vfs::clearAttributes",
            &Client::Stub::clearAttributes,
            protobuf::encoded<cc::platform::vfs::Path>(vpath));
    }

}  // namespace platform::vfs::grpc
