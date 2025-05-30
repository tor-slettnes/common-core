// -*- c++ -*-
//==============================================================================
/// @file grpc-serverinterceptors.h++
/// @brief Server-side interceptor to handle C++ exceptions
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/create-unique.h++"

#include <grpcpp/support/server_interceptor.h>

namespace core::grpc
{
    using ::grpc::experimental::InterceptionHookPoints;
    using ::grpc::experimental::Interceptor;
    using ::grpc::experimental::InterceptorBatchMethods;
    using ::grpc::experimental::ServerInterceptorFactoryInterface;
    using ::grpc::experimental::ServerRpcInfo;

    //==========================================================================
    // Logging Interceptor

    class LoggingInterceptor : public Interceptor
    {
    public:
        LoggingInterceptor(ServerRpcInfo* rpc_info);

        void Intercept(InterceptorBatchMethods* methods) override;

    private:
        ServerRpcInfo* rpc_info;
    };

    //==========================================================================
    // Logging Interceptor Factory

    class LoggingInterceptorFactory
        : public ServerInterceptorFactoryInterface,
          public types::enable_create_unique<LoggingInterceptorFactory,
                                             ServerInterceptorFactoryInterface>
    {
    protected:
        Interceptor* CreateServerInterceptor(ServerRpcInfo* rpc_info) override;
    };

    //==========================================================================
    // Exception Handling Interceptor

    class EHInterceptor : public Interceptor
    {
    public:
        void Intercept(InterceptorBatchMethods* methods) override;
    };

    //==========================================================================
    // Exception Handling Interceptor Factory

    class EHInterceptorFactory
        : public ServerInterceptorFactoryInterface,
          public types::enable_create_unique<EHInterceptorFactory,
                                             ServerInterceptorFactoryInterface>
    {
    protected:
        Interceptor* CreateServerInterceptor(ServerRpcInfo* info) override;
    };

}  // namespace core::grpc
