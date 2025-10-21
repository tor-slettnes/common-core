// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-stream.h++
/// @brief Virtual File SysConfig service - I/O stream implementation over gRPC
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/platform/vfs/vfs.grpc.pb.h"  // Generated from `vfs.proto`

#include "vfs-base.h++"
#include "io/streambuffer.h++"
#include "grpc-status.h++"

namespace vfs::grpc
{
    //==========================================================================
    /// @class ClientBuffer

    class ClientBuffer : public core::io::StreamBuffer
    {
    protected:
        int sync() override;
        void set_status(const core::grpc::Status &status);

    public:
        core::grpc::Status status() const;

    protected:
        core::grpc::Status status_;
    };

    //==========================================================================
    /// @class ClientInputBuffer

    class ClientInputBuffer : public ClientBuffer
    {
        using Super = ClientBuffer;
    public:
        using Reader = std::unique_ptr<::grpc::ClientReader<::cc::platform::vfs::FileChunk>>;

    public:
        ClientInputBuffer(Reader &&reader);
        bool read_some(BufferType *buffer) override;

    private:
        Reader reader;
    };

    //==========================================================================
    /// @class ClientOutputBuffer

    class ClientOutputBuffer : public ClientBuffer
    {
        using Super = ClientBuffer;
    public:
        using Writer = std::unique_ptr<::grpc::ClientWriter<::cc::platform::vfs::FileChunk>>;

    public:
        ClientOutputBuffer(Writer &&writer, const Path &vpath);
        ~ClientOutputBuffer();

        bool write_some(const BufferType &buffer) override;
        int sync() override;

    private:
        Writer writer;
        Path vpath;
        std::uint64_t total_written;
    };


    //==========================================================================
    /// @class ClientInputStream

    class ClientInputStream : public std::istream
    {
        using ClientStub = ::cc::platform::vfs::VirtualFileSystem::Stub;

    public:
        ClientInputStream(const std::unique_ptr<ClientStub> &stub,
                          const Path &vpath);

        core::grpc::Status status() const;

    private:
        std::unique_ptr<::grpc::ClientContext> cxt;
        ClientInputBuffer input_buffer;
    };

    //==========================================================================
    /// @class ClientOutputStream

    class ClientOutputStream : public std::ostream
    {
        using ClientStub = ::cc::platform::vfs::VirtualFileSystem::Stub;

    public:
        ClientOutputStream(const std::unique_ptr<ClientStub> &stub,
                           const Path &vpath);

        core::grpc::Status status() const;

    private:
        google::protobuf::Empty empty;
        std::unique_ptr<::grpc::ClientContext> cxt;
        ClientOutputBuffer output_buffer;
    };
}  // namespace vfs::grpc
