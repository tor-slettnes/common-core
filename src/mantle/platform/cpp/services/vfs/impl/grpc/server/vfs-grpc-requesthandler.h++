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

namespace platform::vfs::grpc
{
    class RequestHandler
        : public core::grpc::SignalRequestHandler<cc::platform::vfs::VirtualFileSystem>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::SignalRequestHandler<cc::platform::vfs::VirtualFileSystem>;

    protected:
        RequestHandler();
        RequestHandler(const std::shared_ptr<vfs::ProviderInterface>& provider);

        ::grpc::Status get_contexts(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::vfs::ContextMap* response) override;

        ::grpc::Status get_open_contexts(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::vfs::ContextMap* response) override;

        ::grpc::Status get_context_spec(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::Path* request,
            ::cc::platform::vfs::ContextSpec* response) override;

        ::grpc::Status open_context(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::Path* request,
            ::cc::platform::vfs::ContextSpec* response) override;

        ::grpc::Status close_context(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::Path* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_volume_info(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::cc::platform::vfs::VolumeInfo* response) override;

        ::grpc::Status get_file_info(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::cc::platform::vfs::FileInfo* response) override;

        ::grpc::Status get_directory(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::cc::platform::vfs::Directory* response) override;

        ::grpc::Status locate(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::LocateRequest* request,
            ::cc::platform::vfs::Directory* response) override;

        ::grpc::Status copy(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status move(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status create_folder(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status remove(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::PathRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status read_file(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::Path* request,
            ::grpc::ServerWriter< ::cc::platform::vfs::FileChunk>* writer) override;

        ::grpc::Status write_file(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader< ::cc::platform::vfs::FileChunk>* reader,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_attributes(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::Path* request,
            ::cc::variant::ValueList* response) override;

        ::grpc::Status set_attributes(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::AttributeRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status clear_attributes(
            ::grpc::ServerContext* context,
            const ::cc::platform::vfs::Path* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status watch(
            ::grpc::ServerContext* context,
            const ::cc::signal::Filter* filter,
            ::grpc::ServerWriter< ::cc::platform::vfs::Signal>* writer) override;

    protected:
        std::shared_ptr<vfs::ProviderInterface> provider;
    };
}  // namespace platform::vfs::grpc
