// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-process.h++
/// @brief SysConfig gRPC implementation - process invocaiton
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-process.h++"
#include "sysconfig-grpc-client.h++"

namespace sysconfig::grpc
{
    class ProcessProvider : public ProcessInterface
    {
        using This = ProcessProvider;
        using Super = ProcessInterface;

    public:
        ProcessProvider(const std::shared_ptr<Client> &client);

        void initialize() override;

        InvocationResult invoke_sync(
            const Invocation &invocation,
            const std::string &input) override;

        PID invoke_async(
            const Invocation &invocation,
            const std::string &input) override;

        InvocationResult invoke_finish(
            PID pid,
            const std::string &input) override;

    private:
        std::shared_ptr<Client> client;
    };
}  // namespace sysconfig::grpc
