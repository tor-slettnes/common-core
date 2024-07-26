// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-stream.h++
/// @brief Virtual File System service - I/O stream implementation over gRPC
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs.grpc.pb.h"  // Generated from `vfs.proto`

#include "vfs-provider.h++"
#include "io/streambuffer.h++"
#include "grpc-status.h++"

namespace platform::vfs::grpc
{
    //==========================================================================
    /// @class ClientBuffer

    class ClientBuffer : public core::io::StreamBuffer
    {
    protected:
        int sync() override;
        void set_status(const core::grpc::Status &status);

    protected:
        core::grpc::Status status;
    };

    //==========================================================================
    /// @class ClientInputBuffer

    class ClientInputBuffer : public ClientBuffer
    {
    public:
        using Reader = std::unique_ptr<::grpc::ClientReader<cc::vfs::FileChunk>>;

    public:
        ClientInputBuffer(Reader &&reader);
        bool read_some(std::string *buffer) override;

    private:
        Reader reader;
    };

    //==========================================================================
    /// @class ClientOutputBuffer

    class ClientOutputBuffer : public ClientBuffer
    {
    public:
        using Writer = std::unique_ptr<::grpc::ClientWriter<cc::vfs::FileChunk>>;

    public:
        ClientOutputBuffer(Writer &&writer, const Path &vpath);
        ~ClientOutputBuffer();

        bool write_some(const std::string &buffer) override;

    private:
        Writer writer;
        Path vpath;
        std::uint64_t total_written;
    };

    //==========================================================================
    /// @class ClientInputStream

    class ClientInputStream : public std::istream
    {
        using ClientStub = cc::vfs::VirtualFileSystem::Stub;

    public:
        ClientInputStream(const std::unique_ptr<ClientStub> &stub,
                          const Path &vpath);

    private:
        std::unique_ptr<::grpc::ClientContext> cxt;
        ClientInputBuffer input_buffer;
    };

    //==========================================================================
    /// @class ClientOutputStream

    class ClientOutputStream : public std::ostream
    {
        using ClientStub = cc::vfs::VirtualFileSystem::Stub;

    public:
        ClientOutputStream(const std::unique_ptr<ClientStub> &stub,
                           const Path &vpath);

    private:
        google::protobuf::Empty empty;
        std::unique_ptr<::grpc::ClientContext> cxt;
        ClientOutputBuffer output_buffer;
    };
}  // namespace platform::vfs::grpc
