// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-requesthandler.c++
/// @brief Handle VFS gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-grpc-requesthandler.h++"
#include "vfs-grpc-signalqueue.h++"
#include "protobuf-vfs.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

namespace platform::vfs::grpc
{
    RequestHandler::RequestHandler()
        : provider(platform::vfs::provider.get_shared())
    {
    }

    RequestHandler::RequestHandler(const std::shared_ptr<platform::vfs::Provider> &provider)
        : provider(provider)
    {
    }

    ::grpc::Status RequestHandler::getContexts(
        ::grpc::ServerContext *cxt,
        const protobuf::Empty *,
        ::cc::platform::vfs::ContextMap *response)
    {
        try
        {
            protobuf::encode(
                this->provider->getContexts(),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::getOpenContexts(
        ::grpc::ServerContext *cxt,
        const protobuf::Empty *,
        ::cc::platform::vfs::ContextMap *response)
    {
        try
        {
            protobuf::encode(
                this->provider->getOpenContexts(),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), cxt->peer());
        }
    }

    // Get a single context's specification
    ::grpc::Status RequestHandler::getContextSpec(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *req,
        ::cc::platform::vfs::ContextSpec *resp)
    {
        try
        {
            protobuf::encode_shared(
                this->provider->getContext(req->context(), true),
                resp);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::openContext(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *req,
        ::cc::platform::vfs::ContextSpec *resp)
    {
        try
        {
            protobuf::encode_shared(
                this->provider->openContext(req->context(), true),
                resp);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::closeContext(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *req,
        protobuf::Empty *)
    {
        try
        {
            if (auto c = this->provider->getContext(req->context(), false))
            {
                this->provider->closeContext(c);
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::volumeStats(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *req,
        ::cc::platform::vfs::VolumeStats *response)
    {
        try
        {
            platform::vfs::Path vpath;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*req, &vpath, nullptr, &flags);
            platform::vfs::VolumeStats vstat = this->provider->volumeStats(vpath, flags);
            protobuf::encode(vstat, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::fileStats(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *req,
        ::cc::platform::vfs::FileStats *response)
    {
        try
        {
            platform::vfs::Path vpath;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*req, &vpath, nullptr, &flags);
            platform::vfs::FileStats stat = this->provider->fileStats(vpath, flags);
            protobuf::encode(stat, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::getDirectory(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::PathRequest *req,
        ::cc::platform::vfs::Directory *resp)
    {
        try
        {
            platform::vfs::Path vpath;
            platform::vfs::OperationFlags flags;
            protobuf::decode(*req, &vpath, nullptr, &flags);
            platform::vfs::Directory dir = this->provider->getDirectory(vpath, flags);
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
            platform::vfs::Directory dir = this->provider->getDirectory(vpath, flags);
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

    ::grpc::Status RequestHandler::createFolder(
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
            this->provider->createFolder(target, flags);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::readFile(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *req,
        ::grpc::ServerWriter<::cc::platform::vfs::FileChunk> *writer)
    {
        try
        {
            Path vpath = protobuf::decoded<Path>(*req);
            ReaderRef reader = this->provider->readFile(vpath);
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

    ::grpc::Status RequestHandler::writeFile(
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
                    writer = this->provider->writeFile(vpath);
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

    ::grpc::Status RequestHandler::getAttributes(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *req,
        ::cc::variant::ValueList *resp)
    {
        try
        {
            Path vpath = protobuf::decoded<Path>(*req);
            core::types::KeyValueMap attributes = this->provider->getAttributes(vpath);
            protobuf::encode(attributes, resp);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::setAttributes(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::AttributeRequest *req,
        protobuf::Empty *)
    {
        try
        {
            this->provider->setAttributes(
                protobuf::decoded<Path>(req->path()),
                protobuf::decoded<core::types::KeyValueMap>(req->attributes()));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *req, cxt->peer());
        }
    }

    ::grpc::Status RequestHandler::clearAttributes(
        ::grpc::ServerContext *cxt,
        const ::cc::platform::vfs::Path *req,
        protobuf::Empty *)
    {
        try
        {
            this->provider->clearAttributes(protobuf::decoded<Path>(*req));
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
