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
#include "logging/logging.h++"

namespace platform::vfs::grpc
{
    RequestHandler::RequestHandler()
        : provider(platform::vfs::vfs.get_shared())
    {
    }

    RequestHandler::RequestHandler(const std::shared_ptr<platform::vfs::ProviderInterface> &provider)
        : provider(provider)
    {
    }

    ::grpc::Status RequestHandler::get_contexts(
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

    ::grpc::Status RequestHandler::get_open_contexts(
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
    ::grpc::Status RequestHandler::get_context_spec(
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

    ::grpc::Status RequestHandler::open_context(
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

    ::grpc::Status RequestHandler::close_context(
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

    ::grpc::Status RequestHandler::volume_stats(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        ::cc::platform::vfs::VolumeStats *response)
    {
        try
        {
            platform::vfs::Path vpath;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*request, &vpath, &flags);
            platform::vfs::VolumeStats vstat = this->provider->volume_stats(vpath, flags);
            protobuf::encode(vstat, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::file_stats(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        ::cc::platform::vfs::FileStats *response)
    {
        try
        {
            platform::vfs::Path vpath;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*request, &vpath, &flags);
            platform::vfs::FileStats stat = this->provider->file_stats(vpath, flags);
            protobuf::encode(stat, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::get_directory(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        ::cc::platform::vfs::Directory *response)
    {
        try
        {
            platform::vfs::Path vpath;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*request, &vpath, &flags);
            platform::vfs::Directory dir = this->provider->get_directory(vpath, flags);
            protobuf::encode(dir, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    // Recursively locate files matching naming or attribute patterns
    ::grpc::Status RequestHandler::locate(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::LocateRequest *request,
        ::cc::platform::vfs::Directory *response)
    {
        try
        {
            platform::vfs::Path root;
            std::vector<std::string> filename_masks;
            core::types::TaggedValueList attribute_filters;
            platform::vfs::OperationFlags flags;

            protobuf::decode(
                *request, &root, &filename_masks, &attribute_filters, &flags);

            platform::vfs::Directory dir = this->provider->locate(
                root, filename_masks, attribute_filters, flags);

            protobuf::encode(dir, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::copy(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        protobuf::Empty *)
    {
        try
        {
            platform::vfs::Paths sources;
            platform::vfs::Path target;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*request, &sources, &target, &flags);
            this->provider->copy(sources, target, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::move(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        protobuf::Empty *)
    {
        try
        {
            platform::vfs::Paths sources;
            platform::vfs::Path target;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*request, &sources, &target, &flags);
            this->provider->move(sources, target, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::remove(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        protobuf::Empty *)
    {
        try
        {
            platform::vfs::Path vpath;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*request, &vpath, &flags);
            this->provider->remove(vpath, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::create_folder(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *request,
        protobuf::Empty *)
    {
        try
        {
            platform::vfs::Path vpath;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*request, &vpath, &flags);
            this->provider->create_folder(vpath, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::read_file(
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
            while (reader->peek() != std::char_traits<char>::eof())
            {
                std::streamsize chunksize = reader->rdbuf()->in_avail();
                std::string buf(chunksize, '\0');
                reader->readsome(buf.data(), buf.size());
                chunk.set_data(std::move(buf));
                writer->Write(chunk);
                chunks++;
                total += chunksize;
            }
            reader->sync();
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

    ::grpc::Status RequestHandler::write_file(
        ::grpc::ServerContext *cxt,
        ::grpc::ServerReader<::cc::platform::vfs::FileChunk> *reader,
        protobuf::Empty *)
    {
        platform::vfs::Path vpath;
        UniqueWriter writer;
        ::cc::platform::vfs::FileChunk chunk;
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
                writer->write(chunk.data().data(), chunk.data().length());
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(),
                                 google::protobuf::Empty(),
                                 cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::get_attributes(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *request,
        ::cc::variant::ValueList *response)
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

    ::grpc::Status RequestHandler::set_attributes(
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

    ::grpc::Status RequestHandler::clear_attributes(
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

    ::grpc::Status RequestHandler::watch(
        ::grpc::ServerContext *context,
        const ::cc::signal::Filter *filter,
        ::grpc::ServerWriter<::cc::platform::vfs::Signal> *writer)
    {
        return this->stream_signals<::cc::platform::vfs::Signal, SignalQueue>(
            context,
            filter,
            writer);
    }

}  // namespace platform::vfs::grpc
