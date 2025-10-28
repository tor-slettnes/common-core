// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-process.c++
/// @brief SysConfig gRPC implementation - process invocaiton
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-grpc-process.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"

namespace sysconfig::grpc
{
    ProcessProvider::ProcessProvider(const std::shared_ptr<Client> &client)
        : Super(TYPE_NAME_FULL(This)),
          client(client)
    {
    }

    void ProcessProvider::initialize()
    {
    }

    InvocationResult ProcessProvider::invoke_sync(
        const Invocation &invocation,
        const std::string &input)
    {
        return protobuf::decoded<InvocationResult>(
            this->client->call_check(
                &Client::Stub::InvokeSync,
                protobuf::encoded<::cc::platform::sysconfig::protobuf::CommandInvocation>(
                    invocation,
                    input)));
    }

    PID ProcessProvider::invoke_async(
        const Invocation &invocation,
        const std::string &input)
    {
        return protobuf::decoded<core::platform::PID>(
            this->client->call_check(
                &Client::Stub::InvokeAsync,
                protobuf::encoded<::cc::platform::sysconfig::protobuf::CommandInvocation>(
                    invocation,
                    input)));
    }

    InvocationResult ProcessProvider::invoke_finish(
        PID pid,
        const std::string &input)
    {
        return protobuf::decoded<InvocationResult>(
            this->client->call_check(
                &Client::Stub::InvokeFinish,
                protobuf::encoded<::cc::platform::sysconfig::protobuf::CommandContinuation>(
                    pid,
                    input)));
    }

}  // namespace sysconfig::grpc
