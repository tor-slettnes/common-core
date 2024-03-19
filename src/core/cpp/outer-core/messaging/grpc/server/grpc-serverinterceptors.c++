// -*- c++ -*-
//==============================================================================
/// @file grpc-serverinterceptors.h++
/// @brief Server-side interceptor to handle C++ exceptions
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "grpc-serverinterceptors.h++"
#include "grpc-status.h++"
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

    void EHInterceptor::Intercept(InterceptorBatchMethods* methods)
    {
        if (methods->QueryInterceptionHookPoint(
                InterceptionHookPoints::PRE_SEND_STATUS))
        {
            log_info("Sending status: ", Status(methods->GetSendStatus()));
        }

        try
        {
            methods->Proceed();
        }
        catch (...)
        {
            Status status(*core::exception::map_to_event(std::current_exception()));
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
