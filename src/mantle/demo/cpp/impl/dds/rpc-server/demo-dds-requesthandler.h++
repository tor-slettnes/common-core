// -*- c++ -*-
//==============================================================================
/// @file demo-dds-requesthandler.h++
/// @brief Handle DemoService RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-interface.hpp"  // generated from `demo-interface.idl`
#include "demo-api.h++"
#include "dds-service.h++"

#include "types/create-shared.h++"

namespace demo::dds
{
    //==========================================================================
    // @brief Process requests from DemoService clients

    class RequestHandler : public CC::Demo::DemoService,
                           public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience alias
        using This = RequestHandler;
        using Super = CC::Demo::DemoService;

    protected:
        RequestHandler(const std::shared_ptr<API> &api_provider);

    public:
        void say_hello(const CC::Demo::Greeting &greeting) override;
        CC::Demo::TimeData get_current_time() override;
        void start_ticking() override;
        void stop_ticking() override;

    private:
        std::shared_ptr<API> provider;
    };
}  // namespace demo::dds
