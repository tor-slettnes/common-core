// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-process.c++
/// @brief SysConfig gRPC implementation - process invocaiton
/// @author Tor Slettnes
//==============================================================================

#include "sysconfig-grpc-process.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"

namespace cc::platform::sysconfig::grpc
{
    ProcessProvider::ProcessProvider(const std::shared_ptr<Client>& client)
        : Super(TYPE_NAME_FULL(This)),
          client(client)
    {
    }

    void ProcessProvider::initialize()
    {
    }

    InvocationResult ProcessProvider::invoke_sync(
        const Invocation& invocation,
        const std::string& input)
    {
        return cc::protobuf::decoded<InvocationResult>(
            this->client->call_check(
                &Client::Stub::InvokeSync,
                cc::protobuf::encoded<protobuf::CommandInvocation>(
                    invocation,
                    input)));
    }

    PID ProcessProvider::invoke_async(
        const Invocation& invocation,
        const std::string& input)
    {
        return cc::protobuf::decoded<core::platform::PID>(
            this->client->call_check(
                &Client::Stub::InvokeAsync,
                cc::protobuf::encoded<protobuf::CommandInvocation>(
                    invocation,
                    input)));
    }

    InvocationResult ProcessProvider::invoke_finish(
        PID pid,
        const std::string& input)
    {
        return cc::protobuf::decoded<InvocationResult>(
            this->client->call_check(
                &Client::Stub::InvokeFinish,
                cc::protobuf::encoded<protobuf::CommandContinuation>(
                    pid,
                    input)));
    }

}  // namespace cc::platform::sysconfig::grpc
