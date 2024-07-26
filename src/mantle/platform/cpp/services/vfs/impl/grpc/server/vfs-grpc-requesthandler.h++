// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-requesthandler.h++
/// @brief Handle VFS gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs.grpc.pb.h"  // Generated from `vfs.proto`
#include "vfs-provider.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

namespace platform::vfs::grpc
{
    class RequestHandler
        : public core::grpc::SignalRequestHandler<cc::vfs::VirtualFileSystem>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::RequestHandler<cc::vfs::VirtualFileSystem>;

    protected:
        RequestHandler();
        RequestHandler(const std::shared_ptr<vfs::Provider>& provider);

        ::grpc::Status getContexts(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::vfs::ContextMap* response) override;

        ::grpc::Status getOpenContexts(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::vfs::ContextMap* response) override;

        ::grpc::Status getContextSpec(
            ::grpc::ServerContext* context,
            const ::cc::vfs::Path* request,
            ::cc::vfs::ContextSpec* response) override;

        ::grpc::Status openContext(
            ::grpc::ServerContext* context,
            const ::cc::vfs::Path* request,
            ::cc::vfs::ContextSpec* response) override;

        ::grpc::Status closeContext(
            ::grpc::ServerContext* context,
            const ::cc::vfs::Path* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status volumeStats(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::cc::vfs::VolumeStats* response) override;

        ::grpc::Status fileStats(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::cc::vfs::FileStats* response) override;

        ::grpc::Status getDirectory(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::cc::vfs::Directory* response) override;

        ::grpc::Status list(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::cc::vfs::DirectoryList* response) override;

        ::grpc::Status locate(
            ::grpc::ServerContext* context,
            const ::cc::vfs::LocateRequest* request,
            ::cc::vfs::Directory* response) override;

        ::grpc::Status copy(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status move(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status createFolder(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status remove(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status readFile(
            ::grpc::ServerContext* context,
            const ::cc::vfs::Path* request,
            ::grpc::ServerWriter< ::cc::vfs::FileChunk>* writer) override;

        ::grpc::Status writeFile(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader< ::cc::vfs::FileChunk>* reader,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status getAttributes(
            ::grpc::ServerContext* context,
            const ::cc::vfs::Path* request,
            ::cc::variant::ValueList* response) override;

        ::grpc::Status setAttributes(
            ::grpc::ServerContext* context,
            const ::cc::vfs::AttributeRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status clearAttributes(
            ::grpc::ServerContext* context,
            const ::cc::vfs::Path* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status watch(
            ::grpc::ServerContext* context,
            const ::cc::signal::Filter* filter,
            ::grpc::ServerWriter< ::cc::vfs::Signal>* writer) override;

    protected:
        std::shared_ptr<vfs::Provider> provider;
    };
}  // namespace platform::vfs::grpc
