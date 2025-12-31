// -*- c++ -*-
//==============================================================================
/// @file grpc-serverinterceptors.c++
/// @brief Server-side interceptor to handle C++ exceptions
/// @author Tor Slettnes
//==============================================================================

#include "grpc-serverinterceptors.h++"
#include "grpc-status.h++"
#include "protobuf-message.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

#include <iostream>

namespace core::grpc
{
    using ::grpc::experimental::InterceptionHookPoints;
    using ::grpc::experimental::Interceptor;
    using ::grpc::experimental::InterceptorBatchMethods;
    using ::grpc::experimental::ServerInterceptorFactoryInterface;
    using ::grpc::experimental::ServerRpcInfo;

    //==========================================================================
    // Exception Handling Interceptor

    LoggingInterceptor::LoggingInterceptor(ServerRpcInfo* rpc_info)
        : Interceptor(),
          rpc_info(rpc_info)
    {
    }

    void LoggingInterceptor::Intercept(InterceptorBatchMethods* methods)
    {
        if (methods->QueryInterceptionHookPoint(
                InterceptionHookPoints::POST_RECV_MESSAGE))
        {
            if (void* data = methods->GetRecvMessage())
            {
                auto* message = static_cast<google::protobuf::Message*>(data);
                if (this->rpc_info && message)
                {
                    logf_trace("Received invocation from %s: %s; inputs: %s",
                               core::str::url_decoded(this->rpc_info->server_context()->peer()),
                               this->rpc_info->method(),
                               *message);
                }
            }
        }

        if (methods->QueryInterceptionHookPoint(
                InterceptionHookPoints::PRE_SEND_STATUS))
        {
            if (this->rpc_info)
            {
                Status status(methods->GetSendStatus());
                logf_trace("Response status to %s: %s",
                           core::str::url_decoded(this->rpc_info->server_context()->peer()),
                           status);
            }
        }

        methods->Proceed();
    }

    //==========================================================================
    // Exception Handling Interceptor Factory

    Interceptor* LoggingInterceptorFactory::CreateServerInterceptor(ServerRpcInfo* rpc_info)
    {
        return new LoggingInterceptor(rpc_info);
    }

    //==========================================================================
    // Exception Handling Interceptor

    void EHInterceptor::Intercept(InterceptorBatchMethods* methods)
    {
        if (methods->QueryInterceptionHookPoint(
                InterceptionHookPoints::PRE_SEND_STATUS))
        {
            log_trace("Sending status: ", Status(methods->GetSendStatus()));
        }

        try
        {
            methods->Proceed();
        }
        catch (...)
        {
            Status status(*core::exception::map_to_error(std::current_exception()));
            log_error("Request failed: ", status);
            methods->ModifySendStatus(status);
        }
    }

    //==========================================================================
    // Exception Handling Interceptor Factory

    Interceptor* EHInterceptorFactory::CreateServerInterceptor(ServerRpcInfo* info)
    {
        return new EHInterceptor();
    }

}  // namespace core::grpc
