// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-requesthandler.h++
/// @brief Handle VFS gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs.grpc.pb.h"  // Generated from `vfs.proto`
#include "vfs-base.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

namespace vfs::grpc
{
    class RequestHandler
        : public core::grpc::SignalRequestHandler<::cc::vfs::VirtualFileSystem>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::SignalRequestHandler<::cc::vfs::VirtualFileSystem>;

    protected:
        RequestHandler();
        RequestHandler(const std::shared_ptr<vfs::ProviderInterface>& provider);

        ::grpc::Status get_contexts(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::vfs::ContextMap* response) override;

        ::grpc::Status get_open_contexts(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::vfs::ContextMap* response) override;

        ::grpc::Status get_context_spec(
            ::grpc::ServerContext* context,
            const ::cc::vfs::Path* request,
            ::cc::vfs::ContextSpec* response) override;

        ::grpc::Status open_context(
            ::grpc::ServerContext* context,
            const ::cc::vfs::Path* request,
            ::cc::vfs::ContextSpec* response) override;

        ::grpc::Status close_context(
            ::grpc::ServerContext* context,
            const ::cc::vfs::Path* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_volume_info(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::cc::vfs::VolumeInfo* response) override;

        ::grpc::Status get_file_info(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::cc::vfs::FileInfo* response) override;

        ::grpc::Status get_directory(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::cc::vfs::Directory* response) override;

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

        ::grpc::Status create_folder(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status remove(
            ::grpc::ServerContext* context,
            const ::cc::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status read_file(
            ::grpc::ServerContext* context,
            const ::cc::vfs::Path* request,
            ::grpc::ServerWriter< ::cc::vfs::FileChunk>* writer) override;

        ::grpc::Status write_file(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader< ::cc::vfs::FileChunk>* reader,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_attributes(
            ::grpc::ServerContext* context,
            const ::cc::vfs::Path* request,
            ::cc::variant::KeyValueMap* response) override;

        ::grpc::Status set_attributes(
            ::grpc::ServerContext* context,
            const ::cc::vfs::AttributeRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status clear_attributes(
            ::grpc::ServerContext* context,
            const ::cc::vfs::Path* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status watch(
            ::grpc::ServerContext* context,
            const ::cc::signal::Filter* filter,
            ::grpc::ServerWriter< ::cc::vfs::Signal>* writer) override;

    protected:
        std::shared_ptr<vfs::ProviderInterface> provider;
    };
}  // namespace vfs::grpc
