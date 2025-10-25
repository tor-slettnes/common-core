// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-requesthandler.h++
/// @brief Handle VFS gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/platform/vfs/vfs.grpc.pb.h"  // Generated from `vfs.proto`
#include "vfs-base.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

namespace vfs::grpc
{
    class RequestHandler
        : public core::grpc::SignalRequestHandler<::cc::platform::vfs::VirtualFileSystem>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::SignalRequestHandler<::cc::platform::vfs::VirtualFileSystem>;

    protected:
        RequestHandler();
        RequestHandler(const std::shared_ptr<vfs::ProviderInterface>& provider);

        ::grpc::Status GetContexts(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::vfs::ContextMap* response) override;

        ::grpc::Status GetOpenContexts(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::vfs::ContextMap* response) override;

        ::grpc::Status GetContextSpec(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::Path* request,
            ::cc::platform::vfs::ContextSpec* response) override;

        ::grpc::Status OpenContext(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::Path* request,
            ::cc::platform::vfs::ContextSpec* response) override;

        ::grpc::Status CloseContext(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::Path* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status GetVolumeInfo(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::cc::platform::vfs::VolumeInfo* response) override;

        ::grpc::Status GetFileInfo(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::cc::platform::vfs::FileInfo* response) override;

        ::grpc::Status GetDirectory(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::cc::platform::vfs::Directory* response) override;

        ::grpc::Status Locate(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::LocateRequest* request,
            ::cc::platform::vfs::Directory* response) override;

        ::grpc::Status Copy(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status Move(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status CreateFolder(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status Remove(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status ReadFile(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::Path* request,
            ::grpc::ServerWriter< ::cc::platform::vfs::FileChunk>* writer) override;

        ::grpc::Status WriteFile(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader< ::cc::platform::vfs::FileChunk>* reader,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status GetAttributes(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::Path* request,
            ::cc::protobuf::variant::KeyValueMap* response) override;

        ::grpc::Status SetAttributes(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::AttributeRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status ClearAttributes(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::Path* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status Watch(
            ::grpc::ServerContext* context,
            const ::cc::protobuf::signal::Filter* filter,
            ::grpc::ServerWriter< ::cc::platform::vfs::Signal>* writer) override;

    protected:
        std::shared_ptr<vfs::ProviderInterface> provider;
    };
}  // namespace vfs::grpc
