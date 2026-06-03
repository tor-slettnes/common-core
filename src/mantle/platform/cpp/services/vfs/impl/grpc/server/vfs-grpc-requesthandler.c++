// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-requesthandler.c++
/// @brief Handle VFS gRPC service requests
/// @author Tor Slettnes
//==============================================================================

#include "vfs-grpc-requesthandler.h++"
#include "vfs-grpc-signalqueue.h++"
#include "protobuf-vfs-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"
#include "protobuf-message.h++"
#include "logging/logging.h++"

namespace cc::platform::vfs::grpc
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
        const vfs::protobuf::GetContextsRequest *request,
        vfs::protobuf::ContextMap *response)
    {
        try
        {
            const ContextMap &contexts = this->provider->get_contexts(
                request->removable_only(),
                request->open_only());

            cc::protobuf::encode(contexts, response);
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
        const vfs::protobuf::Path *request,
        vfs::protobuf::ContextSpec *response)
    {
        try
        {
            cc::protobuf::encode_shared(
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
        const vfs::protobuf::Path *request,
        vfs::protobuf::ContextSpec *response)
    {
        try
        {
            cc::protobuf::encode_shared(
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
        const vfs::protobuf::Path *request,
        cc::protobuf::Empty *)
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
        const vfs::protobuf::PathRequest *request,
        vfs::protobuf::VolumeInfo *response)
    {
        try
        {
            vfs::Path vpath;
            vfs::OperationFlags flags;
            cc::protobuf::decode(*request, &vpath, &flags);
            vfs::VolumeInfo vstat = this->provider->get_volume_info(vpath, flags);
            cc::protobuf::encode(vstat, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::GetFileInfo(
        ::grpc::ServerContext *cxt,
        const vfs::protobuf::PathRequest *request,
        vfs::protobuf::FileInfo *response)
    {
        try
        {
            vfs::Path vpath;
            vfs::OperationFlags flags;
            cc::protobuf::decode(*request, &vpath, &flags);
            vfs::FileInfo stat = this->provider->get_file_info(vpath, flags);
            cc::protobuf::encode(stat, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::GetDirectory(
        ::grpc::ServerContext *cxt,
        const vfs::protobuf::PathRequest *request,
        vfs::protobuf::Directory *response)
    {
        try
        {
            vfs::Path vpath;
            vfs::OperationFlags flags;
            cc::protobuf::decode(*request, &vpath, &flags);
            vfs::Directory dir = this->provider->get_directory(vpath, flags);
            cc::protobuf::encode(dir, response);
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
        const vfs::protobuf::LocateRequest *request,
        vfs::protobuf::Directory *response)
    {
        try
        {
            vfs::Path root;
            core::types::PathList filename_masks;
            core::types::TaggedValueList attribute_filters;
            vfs::OperationFlags flags;

            cc::protobuf::decode(
                *request, &root, &filename_masks, &attribute_filters, &flags);

            vfs::Directory dir = this->provider->locate(
                root, filename_masks, attribute_filters, flags);

            cc::protobuf::encode(dir, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::Copy(
        ::grpc::ServerContext *cxt,
        const vfs::protobuf::PathRequest *request,
        cc::protobuf::Empty *)
    {
        try
        {
            vfs::Paths sources;
            vfs::Path target;
            vfs::OperationFlags flags;
            cc::protobuf::decode(*request, &sources, &target, &flags);
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
        const vfs::protobuf::PathRequest *request,
        cc::protobuf::Empty *)
    {
        try
        {
            vfs::Paths sources;
            vfs::Path target;
            vfs::OperationFlags flags;
            cc::protobuf::decode(*request, &sources, &target, &flags);
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
        const vfs::protobuf::PathRequest *request,
        cc::protobuf::Empty *)
    {
        try
        {
            vfs::Path vpath;
            vfs::Paths vpaths;
            vfs::OperationFlags flags;
            cc::protobuf::decode(*request, &vpaths, &vpath, &flags);
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
        const vfs::protobuf::PathRequest *request,
        cc::protobuf::Empty *)
    {
        try
        {
            vfs::Path vpath;
            vfs::OperationFlags flags;
            cc::protobuf::decode(*request, &vpath, &flags);
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
        const vfs::protobuf::Path *request,
        ::grpc::ServerWriter<vfs::protobuf::FileChunk> *writer)
    {
        try
        {
            Path vpath = cc::protobuf::decoded<Path>(*request);
            UniqueReader reader = this->provider->read_file(vpath);

            reader->exceptions(std::ios::badbit);

            //char buf[protobuf::chunksize];
            vfs::protobuf::FileChunk chunk;
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
        ::grpc::ServerReader<vfs::protobuf::FileChunk> *reader,
        cc::protobuf::Empty *)
    {
        vfs::Path vpath;
        UniqueWriter writer;
        vfs::protobuf::FileChunk chunk;

        std::streamsize total = 0;
        uint chunks = 0;
        try
        {
            while (reader->Read(&chunk))
            {
                if (!writer)
                {
                    cc::protobuf::decode(chunk.path(), &vpath);
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
        const vfs::protobuf::Path *request,
        cc::protobuf::variant::KeyValueMap *response)
    {
        try
        {
            cc::protobuf::encode(
                this->provider->get_attributes(
                    cc::protobuf::decoded<Path>(*request)),
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
        const vfs::protobuf::AttributeRequest *request,
        cc::protobuf::Empty *)
    {
        try
        {
            this->provider->set_attributes(
                cc::protobuf::decoded<Path>(request->path()),
                cc::protobuf::decoded<core::types::KeyValueMap>(request->attributes()));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::ClearAttributes(
        ::grpc::ServerContext *cxt,
        const vfs::protobuf::Path *request,
        cc::protobuf::Empty *)
    {
        try
        {
            this->provider->clear_attributes(cc::protobuf::decoded<Path>(*request));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::Watch(
        ::grpc::ServerContext *context,
        const cc::protobuf::signal::Filter *filter,
        ::grpc::ServerWriter<vfs::protobuf::Signal> *writer)
    {
        return this->stream_signals<vfs::protobuf::Signal, SignalQueue>(
            context,
            filter,
            writer);
    }

}  // namespace cc::platform::vfs::grpc
