// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-provider.c++
/// @brief Virtual File SysConfig service - gRPC client implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-grpc-provider.h++"
#include "vfs-grpc-stream.h++"
#include "vfs-remote-context.h++"
#include "protobuf-vfs-types.h++"
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

    void ClientProvider::set_use_cached(bool use_cached)
    {
        this->use_cached = use_cached;
    }

    bool ClientProvider::get_use_cached() const
    {
        return this->use_cached;
    }

    ContextMap ClientProvider::get_contexts() const
    {
        if (this->get_use_cached())
        {
            return platform::vfs::signal_context.get_cached();
        }
        else
        {
            return protobuf::decoded<ContextMap>(
                this->client->call_check("vfs::get_contexts",
                                         &Client::Stub::get_contexts));
        }
    }

    ContextMap ClientProvider::get_open_context() const
    {
        if (this->get_use_cached())
        {
            return platform::vfs::signal_context_in_use.get_cached();
        }
        else
        {
            return protobuf::decoded<ContextMap>(
                this->client->call_check("vfs::get_open_context",
                                         &Client::Stub::get_open_context));
        }
    }

    ContextRef ClientProvider::get_context(const std::string &name, bool required) const
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
                    this->client->call_check("vfs::get_context_spec",
                                             &Client::Stub::get_context_spec,
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

    ContextRef ClientProvider::open_context(const std::string &name, bool required)
    {
        cc::platform::vfs::Path request;
        request.set_context(name);
        try
        {
            return protobuf::decode_shared<RemoteContext>(
                this->client->call_check("vfs::open_context",
                                         &Client::Stub::open_context,
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

    void ClientProvider::close_context(const ContextRef &cxt)
    {
        if (cxt)
        {
            cc::platform::vfs::Path request;
            request.set_context(cxt->name);

            try
            {
                this->client->call_check("vfs::close_context",
                                         &Client::Stub::close_context,
                                         request);
            }
            catch (const std::exception &e)
            {
                logf_notice("Failed to close context %r: %r", cxt->name, e);
            }
        }
    }

    VolumeStats ClientProvider::volume_stats(const Path &vpath,
                                             const OperationFlags &flags) const
    {
        return protobuf::decoded<VolumeStats>(
            this->client->call_check(
                "vfs::volume_stats",
                &Client::Stub::volume_stats,
                protobuf::encoded<cc::platform::vfs::PathRequest>(vpath, nullpath, flags)));
    }

    FileStats ClientProvider::file_stats(const Path &vpath,
                                         const OperationFlags &flags) const
    {
        return protobuf::decoded<FileStats>(
            this->client->call_check(
                "vfs::file_stats",
                &Client::Stub::file_stats,
                protobuf::encoded<cc::platform::vfs::PathRequest>(vpath, nullpath, flags)));
    }

    Directory ClientProvider::get_directory(const Path &vpath,
                                            const OperationFlags &flags) const
    {
        return protobuf::decoded<Directory>(
            this->client->call_check(
                "vfs::get_directory",
                &Client::Stub::get_directory,
                protobuf::encoded<cc::platform::vfs::PathRequest>(vpath, nullpath, flags)));
    }

    Directory ClientProvider::locate(const Path &root,
                                     const std::vector<std::string> &filename_masks,
                                     const core::types::TaggedValueList &attribute_filters,
                                     const OperationFlags &flags) const
    {
        return protobuf::decoded<Directory>(
            this->client->call_check(
                "vfs::locate",
                &Client::Stub::locate,
                protobuf::encoded<cc::platform::vfs::LocateRequest>(
                    root, filename_masks, attribute_filters, flags)));
    }

    void ClientProvider::copy(const Paths &sources,
                              const Path &target,
                              const OperationFlags &flags) const
    {
        this->client->call_check(
            "vfs::copy",
            &Client::Stub::copy,
            protobuf::encoded<cc::platform::vfs::PathRequest>(sources, target, flags));
    }

    void ClientProvider::move(const Paths &sources,
                              const Path &target,
                              const OperationFlags &flags) const
    {
        this->client->call_check(
            "vfs::move",
            &Client::Stub::move,
            protobuf::encoded<cc::platform::vfs::PathRequest>(sources, target, flags));
    }

    void ClientProvider::remove(const Paths &sources,
                                const OperationFlags &flags) const
    {
        this->client->call_check(
            "vfs::remove",
            &Client::Stub::remove,
            protobuf::encoded<cc::platform::vfs::PathRequest>(sources, nullpath, flags));
    }

    void ClientProvider::create_folder(const Path &vpath,
                                       const OperationFlags &flags) const
    {
        this->client->call_check(
            "vfs::create_folder",
            &Client::Stub::create_folder,
            protobuf::encoded<cc::platform::vfs::PathRequest>(nullpath, vpath, flags));
    }

    ReaderRef ClientProvider::read_file(const Path &vpath) const
    {
        return std::make_unique<ClientInputStream>(this->client->stub, vpath);
    }

    WriterRef ClientProvider::write_file(const Path &vpath) const
    {
        return std::make_unique<ClientOutputStream>(this->client->stub, vpath);
    }

    core::types::KeyValueMap ClientProvider::get_attributes(const Path &vpath) const
    {
        return protobuf::decoded<core::types::KeyValueMap>(
            this->client->call_check(
                "vfs::get_attributes",
                &Client::Stub::get_attributes,
                protobuf::encoded<cc::platform::vfs::Path>(vpath)));
    }

    void ClientProvider::set_attributes(const Path &vpath,
                                        const core::types::KeyValueMap &attributes) const
    {
        this->client->call_check(
            "vfs::set_attributes",
            &Client::Stub::set_attributes,
            protobuf::encoded<cc::platform::vfs::AttributeRequest>(vpath, attributes));
    }

    void ClientProvider::clear_attributes(const Path &vpath) const
    {
        this->client->call_check(
            "vfs::clear_attributes",
            &Client::Stub::clear_attributes,
            protobuf::encoded<cc::platform::vfs::Path>(vpath));
    }

}  // namespace platform::vfs::grpc
