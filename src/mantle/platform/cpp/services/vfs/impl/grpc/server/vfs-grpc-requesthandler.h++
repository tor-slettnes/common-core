// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-requesthandler.h++
/// @brief Handle VFS gRPC service requests
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "vfs-base.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

#include "cc/platform/vfs/grpc/vfs_service.grpc.pb.h"

namespace cc::platform::vfs
{
    using namespace cc::platform::vfs;
}

namespace cc::platform::vfs::grpc
{
    class RequestHandler
        : public cc::grpc::SignalRequestHandler<cc::platform::vfs::grpc::VirtualFileSystem>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = cc::grpc::SignalRequestHandler<cc::platform::vfs::grpc::VirtualFileSystem>;

    protected:
        RequestHandler();
        RequestHandler(const std::shared_ptr<vfs::ProviderInterface>& provider);

        ::grpc::Status ServiceCheck(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ServiceCheckResponse *response) override;

        ::grpc::Status GetContexts(
            ::grpc::ServerContext* context,
            const vfs::protobuf::GetContextsRequest* request,
            vfs::protobuf::ContextMap* response) override;

        ::grpc::Status GetContextSpec(
            ::grpc::ServerContext* context,
            const vfs::protobuf::Path* request,
            vfs::protobuf::ContextSpec* response) override;

        ::grpc::Status OpenContext(
            ::grpc::ServerContext* context,
            const vfs::protobuf::Path* request,
            vfs::protobuf::ContextSpec* response) override;

        ::grpc::Status CloseContext(
            ::grpc::ServerContext* context,
            const vfs::protobuf::Path* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status GetVolumeInfo(
            ::grpc::ServerContext* context,
            const vfs::protobuf::PathRequest* request,
            vfs::protobuf::VolumeInfo* response) override;

        ::grpc::Status GetFileInfo(
            ::grpc::ServerContext* context,
            const vfs::protobuf::PathRequest* request,
            vfs::protobuf::FileInfo* response) override;

        ::grpc::Status GetDirectory(
            ::grpc::ServerContext* context,
            const vfs::protobuf::PathRequest* request,
            vfs::protobuf::Directory* response) override;

        ::grpc::Status Locate(
            ::grpc::ServerContext* context,
            const vfs::protobuf::LocateRequest* request,
            vfs::protobuf::Directory* response) override;

        ::grpc::Status Copy(
            ::grpc::ServerContext* context,
            const vfs::protobuf::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status Move(
            ::grpc::ServerContext* context,
            const vfs::protobuf::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status CreateFolder(
            ::grpc::ServerContext* context,
            const vfs::protobuf::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status Remove(
            ::grpc::ServerContext* context,
            const vfs::protobuf::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status ReadFile(
            ::grpc::ServerContext* context,
            const vfs::protobuf::Path* request,
            ::grpc::ServerWriter<vfs::protobuf::FileChunk>* writer) override;

        ::grpc::Status WriteFile(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader<vfs::protobuf::FileChunk>* reader,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status GetAttributes(
            ::grpc::ServerContext* context,
            const vfs::protobuf::Path* request,
            cc::protobuf::variant::KeyValueMap* response) override;

        ::grpc::Status SetAttributes(
            ::grpc::ServerContext* context,
            const vfs::protobuf::AttributeRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status ClearAttributes(
            ::grpc::ServerContext* context,
            const vfs::protobuf::Path* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status Watch(
            ::grpc::ServerContext* context,
            const cc::protobuf::signal::Filter* filter,
            ::grpc::ServerWriter<vfs::protobuf::Signal>* writer) override;

    protected:
        std::shared_ptr<vfs::ProviderInterface> provider;
    };
}  // namespace cc::platform::vfs::grpc
