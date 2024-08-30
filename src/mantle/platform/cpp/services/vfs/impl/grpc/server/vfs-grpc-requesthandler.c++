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

    ::grpc::Status RequestHandler::get_open_context(
        ::grpc::ServerContext *cxt,
        const protobuf::Empty *,
        ::cc::platform::vfs::ContextMap *response)
    {
        try
        {
            protobuf::encode(
                this->provider->get_open_context(),
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
        const ::cc::platform::vfs::Path *req,
        ::cc::platform::vfs::ContextSpec *resp)
    {
        try
        {
            protobuf::encode_shared(
                this->provider->get_context(req->context(), true),
                resp);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::open_context(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *req,
        ::cc::platform::vfs::ContextSpec *resp)
    {
        try
        {
            protobuf::encode_shared(
                this->provider->open_context(req->context(), true),
                resp);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::close_context(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *req,
        protobuf::Empty *)
    {
        try
        {
            if (auto c = this->provider->get_context(req->context(), false))
            {
                this->provider->close_context(c);
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::volume_stats(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *req,
        ::cc::platform::vfs::VolumeStats *response)
    {
        try
        {
            platform::vfs::Path vpath;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*req, &vpath, nullptr, &flags);
            platform::vfs::VolumeStats vstat = this->provider->volume_stats(vpath, flags);
            protobuf::encode(vstat, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::file_stats(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *req,
        ::cc::platform::vfs::FileStats *response)
    {
        try
        {
            platform::vfs::Path vpath;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*req, &vpath, nullptr, &flags);
            platform::vfs::FileStats stat = this->provider->file_stats(vpath, flags);
            protobuf::encode(stat, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::get_directory(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *req,
        ::cc::platform::vfs::Directory *resp)
    {
        try
        {
            platform::vfs::Path vpath;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*req, &vpath, nullptr, &flags);
            platform::vfs::Directory dir = this->provider->get_directory(vpath, flags);
            protobuf::encode(dir, resp);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::list(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *req,
        ::cc::platform::vfs::DirectoryList *resp)
    {
        try
        {
            platform::vfs::Path vpath;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*req, &vpath, nullptr, &flags);
            platform::vfs::Directory dir = this->provider->get_directory(vpath, flags);
            protobuf::encode(dir, resp);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
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
        const ::cc::platform::vfs::PathRequest *req,
        protobuf::Empty *)
    {
        try
        {
            platform::vfs::Paths sources;
            platform::vfs::Path target;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*req, &sources, &target, &flags);
            this->provider->copy(sources, target, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::move(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *req,
        protobuf::Empty *)
    {
        try
        {
            platform::vfs::Paths sources;
            platform::vfs::Path target;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*req, &sources, &target, &flags);
            this->provider->move(sources, target, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::remove(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *req,
        protobuf::Empty *)
    {
        try
        {
            platform::vfs::Paths sources;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*req, &sources, nullptr, &flags);
            this->provider->remove(sources, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::create_folder(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *req,
        protobuf::Empty *)
    {
        try
        {
            platform::vfs::Paths sources;
            platform::vfs::Path target;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*req, &sources, &target, &flags);
            this->provider->create_folder(target, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::read_file(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *req,
        ::grpc::ServerWriter<::cc::platform::vfs::FileChunk> *writer)
    {
        try
        {
            Path vpath = protobuf::decoded<Path>(*req);
            ReaderRef reader = this->provider->read_file(vpath);
            reader->exceptions(std::ios::badbit);

            //char buf[protobuf::chunksize];
            ::cc::platform::vfs::FileChunk chunk;
            chunk.mutable_path()->CopyFrom(*req);
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
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::write_file(
        ::grpc::ServerContext *cxt,
        ::grpc::ServerReader<::cc::platform::vfs::FileChunk> *reader,
        protobuf::Empty *)
    {
        platform::vfs::Path vpath;
        WriterRef writer;
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
        const ::cc::platform::vfs::Path *req,
        ::cc::variant::ValueList *resp)
    {
        try
        {
            Path vpath = protobuf::decoded<Path>(*req);
            core::types::KeyValueMap attributes = this->provider->get_attributes(vpath);
            protobuf::encode(attributes, resp);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::set_attributes(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::AttributeRequest *req,
        protobuf::Empty *)
    {
        try
        {
            this->provider->set_attributes(
                protobuf::decoded<Path>(req->path()),
                protobuf::decoded<core::types::KeyValueMap>(req->attributes()));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::clear_attributes(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *req,
        protobuf::Empty *)
    {
        try
        {
            this->provider->clear_attributes(protobuf::decoded<Path>(*req));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
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
