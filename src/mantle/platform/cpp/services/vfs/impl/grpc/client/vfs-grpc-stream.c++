// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-stream.c++
/// @brief Virtual File SysConfig service - I/O stream implementation over gRPC
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-grpc-stream.h++"
#include "protobuf-vfs-types.h++"
#include "protobuf-inline.h++"

namespace vfs::grpc
{
    //==========================================================================
    /// @class ClientBuffer

    int ClientBuffer::sync()
    {
        return this->status().ok() ? 0 : -1;
    }

    void ClientBuffer::set_status(const core::grpc::Status &status)
    {
        this->status_ = status;
    }

    core::grpc::Status ClientBuffer::status() const
    {
        return this->status_;
    }

    //==========================================================================
    /// @class ClientInputBuffer

    ClientInputBuffer::ClientInputBuffer(Reader &&reader)
        : reader(std::move(reader))
    {
    }

    bool ClientInputBuffer::read_some(BufferType *buffer)
    {
        ::cc::platform::vfs::FileChunk msg;
        if (this->reader->Read(&msg))
        {
            buffer->assign(msg.data());
            return true;
        }
        else
        {
            buffer->clear();
            this->set_status(this->reader->Finish());
            return false;
        }
    }

    //==========================================================================
    /// @class ClientOutputBuffer

    ClientOutputBuffer::ClientOutputBuffer(
        Writer &&writer,
        const vfs::Path &vpath)
        : writer(std::move(writer)),
          vpath(vpath),
          total_written(0)
    {
    }

    ClientOutputBuffer::~ClientOutputBuffer()
    {
        this->sync();
        logf_debug("Wrote %d bytes to VFS path %s",
                   this->total_written,
                   this->vpath);
    }

    bool ClientOutputBuffer::write_some(const BufferType &data)
    {
        ::cc::platform::vfs::FileChunk msg;
        protobuf::encode(this->vpath, msg.mutable_path());
        msg.set_data(data);

        if (this->writer->Write(msg))
        {
            this->total_written += data.size();
            return true;
        }
        else
        {
            this->set_status(this->writer->Finish());
            return false;
        }
    }

    int ClientOutputBuffer::sync()
    {
        this->writer->WritesDone();
        this->set_status(this->writer->Finish());
        return Super::sync();
    }

    //==========================================================================
    /// @class ClientInputStream

    ClientInputStream::ClientInputStream(const std::unique_ptr<ClientStub> &stub,
                                         const Path &vpath)
        : cxt(std::make_unique<::grpc::ClientContext>()),
          input_buffer(
              stub->ReadFile(
                  cxt.get(),
                  protobuf::encoded<::cc::platform::vfs::Path>(vpath)))
    {
        this->rdbuf(&this->input_buffer);
    }

    core::grpc::Status ClientInputStream::status() const
    {
        return this->input_buffer.status();
    }

    //==========================================================================

    /// @class ClientOutputStream

    ClientOutputStream::ClientOutputStream(const std::unique_ptr<ClientStub> &stub,
                                           const Path &vpath)
        : cxt(std::make_unique<::grpc::ClientContext>()),
          output_buffer(
              stub->WriteFile(cxt.get(), &empty),
              vpath)
    {
        this->rdbuf(&this->output_buffer);
    }

    core::grpc::Status ClientOutputStream::status() const
    {
        return this->output_buffer.status();
    }

}  // namespace vfs::grpc
