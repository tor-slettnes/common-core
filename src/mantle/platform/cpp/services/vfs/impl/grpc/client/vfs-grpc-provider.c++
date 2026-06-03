// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-provider.c++
/// @brief Virtual File SysConfig service - gRPC client implementation
/// @author Tor Slettnes
//==============================================================================

#include "vfs-grpc-provider.h++"
#include "vfs-grpc-stream.h++"
#include "protobuf-vfs-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

namespace cc::platform::vfs::grpc
{
    const vfs::Path nullpath;

    ClientProvider::ClientProvider(const std::shared_ptr<Client> &client,
                                   bool use_cached)
        : Super("vfs::ClientProvider"),
          client(client),
          use_cached(use_cached)
    {
    }

    bool ClientProvider::is_pertinent() const
    {
        return bool(client);
    }

    void ClientProvider::initialize()
    {
        Super::initialize();

        using namespace std::placeholders;

        this->client->add_mapping_handler(
            platform::vfs::protobuf::Signal::kContext,
            std::bind(&This::on_context, this, _1, _2, _3));

        this->client->add_mapping_handler(
            platform::vfs::protobuf::Signal::kContextInUse,
            std::bind(&This::on_context_in_use, this, _1, _2, _3));
    }

    void ClientProvider::set_use_cached(bool use_cached)
    {
        this->use_cached = use_cached;
    }

    bool ClientProvider::get_use_cached() const
    {
        return this->use_cached;
    }

    ContextMap ClientProvider::get_contexts(
        bool removable_only,
        bool open_only) const
    {
        if (this->get_use_cached() && !removable_only && !open_only)
        {
            return vfs::signal_context.get_cached();
        }
        else
        {
            platform::vfs::protobuf::GetContextsRequest request;
            request.set_removable_only(removable_only);
            request.set_open_only(open_only);

            return this->context_map(
                this->client->call_check(
                    &Client::Stub::GetContexts,
                    request));
        }
    }

    Context::ptr ClientProvider::get_context(
        const std::string &name,
        bool required) const
    {
        Context::ptr cxt;
        if (this->get_use_cached())
        {
            cxt = signal_context.get_cached(name, {});
            if (required && !cxt)
            {
                throwf(core::exception::NotFound,
                       "No such filesystem context: %r",
                       name);
            }
        }
        else
        {
            platform::vfs::protobuf::Path request;
            request.set_context(name);

            try
            {
                cxt = this->decoded_context(
                    this->client->call_check(
                        &Client::Stub::GetContextSpec,
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

    Context::ptr ClientProvider::open_context(
        const std::string &name,
        bool required)
    {
        platform::vfs::protobuf::Path request;
        request.set_context(name);
        try
        {
            return this->decoded_context(
                this->client->call_check(
                    &Client::Stub::OpenContext,
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

    void ClientProvider::close_context(
        const std::string &name,
        bool required)
    {
        platform::vfs::protobuf::Path request;
        request.set_context(name);
        try
        {
            this->client->call_check(&Client::Stub::CloseContext, request);
        }
        catch (...)
        {
            logf_info("Failed to close context %r: %r", name, std::current_exception());
            if (required)
            {
                throw;
            }
        }
    }

    void ClientProvider::close_context(
        const Context::ptr &cxt)
    {
        this->close_context(cxt->name, false);
    }

    VolumeInfo ClientProvider::get_volume_info(
        const Path &vpath,
        const OperationFlags &flags) const
    {
        return cc::protobuf::decoded<VolumeInfo>(
            this->client->call_check(
                &Client::Stub::GetVolumeInfo,
                cc::protobuf::encoded<platform::vfs::protobuf::PathRequest>(vpath, flags)));
    }

    FileInfo ClientProvider::get_file_info(
        const Path &vpath,
        const OperationFlags &flags) const
    {
        return cc::protobuf::decoded<FileInfo>(
            this->client->call_check(
                &Client::Stub::GetFileInfo,
                cc::protobuf::encoded<platform::vfs::protobuf::PathRequest>(vpath, flags)));
    }

    Directory ClientProvider::get_directory(
        const Path &vpath,
        const OperationFlags &flags) const
    {
        return cc::protobuf::decoded<Directory>(
            this->client->call_check(
                &Client::Stub::GetDirectory,
                cc::protobuf::encoded<platform::vfs::protobuf::PathRequest>(vpath, flags)));
    }

    Directory ClientProvider::locate(
        const Path &root,
        const core::types::PathList  &filename_masks,
        const core::types::TaggedValueList &attribute_filters,
        const OperationFlags &flags) const
    {
        return cc::protobuf::decoded<Directory>(
            this->client->call_check(
                &Client::Stub::Locate,
                cc::protobuf::encoded<platform::vfs::protobuf::LocateRequest>(
                    root, filename_masks, attribute_filters, flags)));
    }

    void ClientProvider::copy(
        const Paths &sources,
        const Path &target,
        const OperationFlags &flags) const
    {
        this->client->call_check(
            &Client::Stub::Copy,
            cc::protobuf::encoded<platform::vfs::protobuf::PathRequest>(sources, target, flags));
    }

    void ClientProvider::move(
        const Paths &sources,
        const Path &target,
        const OperationFlags &flags) const
    {
        this->client->call_check(
            &Client::Stub::Move,
            cc::protobuf::encoded<platform::vfs::protobuf::PathRequest>(sources, target, flags));
    }

    void ClientProvider::remove(
        const Paths &vpaths,
        const OperationFlags &flags) const
    {
        this->client->call_check(
            &Client::Stub::Remove,
            cc::protobuf::encoded<platform::vfs::protobuf::PathRequest>(vpaths, Path(), flags));
    }

    void ClientProvider::create_folder(
        const Path &vpath,
        const OperationFlags &flags) const
    {
        this->client->call_check(
            &Client::Stub::CreateFolder,
            cc::protobuf::encoded<platform::vfs::protobuf::PathRequest>(vpath, flags));
    }

    UniqueReader ClientProvider::read_file(
        const Path &vpath) const
    {
        return std::make_unique<ClientInputStream>(this->client->stub, vpath);
    }

    UniqueWriter ClientProvider::write_file(
        const Path &vpath) const
    {
        return std::make_unique<ClientOutputStream>(this->client->stub, vpath);
    }

    core::types::KeyValueMap ClientProvider::get_attributes(
        const Path &vpath) const
    {
        return cc::protobuf::decoded<core::types::KeyValueMap>(
            this->client->call_check(
                &Client::Stub::GetAttributes,
                cc::protobuf::encoded<platform::vfs::protobuf::Path>(vpath)));
    }

    void ClientProvider::set_attributes(
        const Path &vpath,
        const core::types::KeyValueMap &attributes) const
    {
        this->client->call_check(
            &Client::Stub::SetAttributes,
            cc::protobuf::encoded<platform::vfs::protobuf::AttributeRequest>(vpath, attributes));
    }

    void ClientProvider::clear_attributes(
        const Path &vpath) const
    {
        this->client->call_check(
            &Client::Stub::ClearAttributes,
            cc::protobuf::encoded<platform::vfs::protobuf::Path>(vpath));
    }

    void ClientProvider::on_context(
        core::signal::MappingAction action,
        const std::string &key,
        const platform::vfs::protobuf::Signal &signal) const
    {
        vfs::signal_context.emit(
            action,
            key,
            this->decoded_context(signal.context()));
    }

    void ClientProvider::on_context_in_use(
        core::signal::MappingAction action,
        const std::string &key,
        const platform::vfs::protobuf::Signal &signal) const
    {
        vfs::signal_context_in_use.emit(
            action,
            key,
            this->decoded_context(signal.context_in_use()));
    }

    ContextMap ClientProvider::context_map(
        const platform::vfs::protobuf::ContextMap &msg) const
    {
        ContextMap map;
        for (const auto &[id, data] : msg.map())
        {
            map.insert_or_assign(id, this->decoded_context(data));
        }
        return map;
    }

    std::shared_ptr<RemoteContext> ClientProvider::decoded_context(
        const platform::vfs::protobuf::ContextSpec &spec) const
    {
        auto cxt = std::make_shared<RemoteContext>(
            const_cast<ClientProvider *>(this)->shared_from_this());

        cc::protobuf::decode(spec, cxt.get());
        return cxt;
    }

}  // namespace cc::platform::vfs::grpc
