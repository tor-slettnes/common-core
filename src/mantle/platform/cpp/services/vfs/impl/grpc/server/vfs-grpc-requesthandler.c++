// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-requesthandler.c++
/// @brief Handle VFS gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-grpc-requesthandler.h++"
#include "vfs-grpc-signalqueue.h++"
#include "protobuf-vfs-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"
#include "protobuf-message.h++"
#include "logging/logging.h++"

namespace vfs::grpc
{
    RequestHandler::RequestHandler()
        : provider(vfs::vfs.get_shared())
    {
    }

    RequestHandler::RequestHandler(const std::shared_ptr<vfs::ProviderInterface> &provider)
        : provider(provider)
    {
    }

    ::grpc::Status RequestHandler::GetContexts(
        ::grpc::ServerContext *cxt,
        const protobuf::Empty *,
        ::cc::platform::vfs::ContextMap *response)
    {
        try
        {
            protobuf::encode(
                this->provider->get_contexts(),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::GetOpenContexts(
        ::grpc::ServerContext *cxt,
        const protobuf::Empty *,
        ::cc::platform::vfs::ContextMap *response)
    {
        try
        {
            protobuf::encode(
                this->provider->get_open_contexts(),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), cxt->peer());
        }
    }

    // Get a single context's specification
    ::grpc::Status RequestHandler::GetContextSpec(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *request,
        ::cc::platform::vfs::ContextSpec *response)
    {
        try
        {
            protobuf::encode_shared(
                this->provider->get_context(request->context(), true),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::OpenContext(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *request,
        ::cc::platform::vfs::ContextSpec *response)
    {
        try
        {
            protobuf::encode_shared(
                this->provider->open_context(request->context(), true),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::CloseContext(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *request,
        protobuf::Empty *)
    {
        try
        {
            if (auto c = this->provider->get_context(request->context(), false))
            {
                this->provider->close_context(c);
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::GetVolumeInfo(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        ::cc::platform::vfs::VolumeInfo *response)
    {
        try
        {
            vfs::Path vpath;
            vfs::OperationFlags flags;
            protobuf::decode(*request, &vpath, &flags);
            vfs::VolumeInfo vstat = this->provider->get_volume_info(vpath, flags);
            protobuf::encode(vstat, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::GetFileInfo(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        ::cc::platform::vfs::FileInfo *response)
    {
        try
        {
            vfs::Path vpath;
            vfs::OperationFlags flags;
            protobuf::decode(*request, &vpath, &flags);
            vfs::FileInfo stat = this->provider->get_file_info(vpath, flags);
            protobuf::encode(stat, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::GetDirectory(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        ::cc::platform::vfs::Directory *response)
    {
        try
        {
            vfs::Path vpath;
            vfs::OperationFlags flags;
            protobuf::decode(*request, &vpath, &flags);
            vfs::Directory dir = this->provider->get_directory(vpath, flags);
            protobuf::encode(dir, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    // Recursively locate files matching naming or attribute patterns
    ::grpc::Status RequestHandler::Locate(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::LocateRequest *request,
        ::cc::platform::vfs::Directory *response)
    {
        try
        {
            vfs::Path root;
            core::types::PathList filename_masks;
            core::types::TaggedValueList attribute_filters;
            vfs::OperationFlags flags;

            protobuf::decode(
                *request, &root, &filename_masks, &attribute_filters, &flags);

            vfs::Directory dir = this->provider->locate(
                root, filename_masks, attribute_filters, flags);

            protobuf::encode(dir, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::Copy(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        protobuf::Empty *)
    {
        try
        {
            vfs::Paths sources;
            vfs::Path target;
            vfs::OperationFlags flags;
            protobuf::decode(*request, &sources, &target, &flags);
            this->provider->copy(sources, target, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::Move(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        protobuf::Empty *)
    {
        try
        {
            vfs::Paths sources;
            vfs::Path target;
            vfs::OperationFlags flags;
            protobuf::decode(*request, &sources, &target, &flags);
            this->provider->move(sources, target, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::Remove(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        protobuf::Empty *)
    {
        try
        {
            vfs::Path vpath;
            vfs::Paths vpaths;
            vfs::OperationFlags flags;
            protobuf::decode(*request, &vpaths, &vpath, &flags);
            if (vpath)
            {
                vpaths.insert(vpaths.begin(), vpath);
            }
            this->provider->remove(vpaths, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::CreateFolder(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        protobuf::Empty *)
    {
        try
        {
            vfs::Path vpath;
            vfs::OperationFlags flags;
            protobuf::decode(*request, &vpath, &flags);
            this->provider->create_folder(vpath, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::ReadFile(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *request,
        ::grpc::ServerWriter<::cc::platform::vfs::FileChunk> *writer)
    {
        try
        {
            Path vpath = protobuf::decoded<Path>(*request);
            UniqueReader reader = this->provider->read_file(vpath);

            reader->exceptions(std::ios::badbit);

            //char buf[protobuf::chunksize];
            ::cc::platform::vfs::FileChunk chunk;
            chunk.mutable_path()->CopyFrom(*request);
            uint chunks = 0;
            std::streamsize total = 0;

            logf_info("reading chunk");

            while (auto bytes = this->provider->read_chunk(*reader))
            {
                chunks++;
                total += bytes->size();
                chunk.set_data(std::move(bytes.value()));
                writer->Write(chunk);
            }

            logf_debug("Sent file %s to client %s in %d chunks, totally %d bytes",
                       vpath,
                       cxt->peer(),
                       chunks,
                       total);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::WriteFile(
        ::grpc::ServerContext *cxt,
        ::grpc::ServerReader<::cc::platform::vfs::FileChunk> *reader,
        protobuf::Empty *)
    {
        vfs::Path vpath;
        UniqueWriter writer;
        ::cc::platform::vfs::FileChunk chunk;

        std::streamsize total = 0;
        uint chunks = 0;
        try
        {
            while (reader->Read(&chunk))
            {
                if (!writer)
                {
                    protobuf::decode(chunk.path(), &vpath);
                    writer = this->provider->write_file(vpath);
                    writer->exceptions(writer->exceptions() |
                                       std::ios::failbit |
                                       std::ios::badbit);
                }

                chunks++;
                total += chunk.data().size();
                this->provider->write_chunk(*writer, chunk.data());
            }
            logf_debug("Received %d bytes in in %d chunks\n", total, chunks);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(),
                                 google::protobuf::Empty(),
                                 cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::GetAttributes(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *request,
        ::cc::protobuf::variant::KeyValueMap *response)
    {
        try
        {
            protobuf::encode(
                this->provider->get_attributes(
                    protobuf::decoded<Path>(*request)),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::SetAttributes(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::AttributeRequest *request,
        protobuf::Empty *)
    {
        try
        {
            this->provider->set_attributes(
                protobuf::decoded<Path>(request->path()),
                protobuf::decoded<core::types::KeyValueMap>(request->attributes()));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::ClearAttributes(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *request,
        protobuf::Empty *)
    {
        try
        {
            this->provider->clear_attributes(protobuf::decoded<Path>(*request));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }


    ::grpc::Status RequestHandler::Watch(
        ::grpc::ServerContext *context,
        const ::cc::protobuf::signal::Filter *filter,
        ::grpc::ServerWriter<::cc::platform::vfs::Signal> *writer)
    {
        return this->stream_signals<::cc::platform::vfs::Signal, SignalQueue>(
            context,
            filter,
            writer);
    }

}  // namespace vfs::grpc
