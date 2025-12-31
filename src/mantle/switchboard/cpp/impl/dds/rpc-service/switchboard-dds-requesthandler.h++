// -*- c++ -*-
//==============================================================================
/// @file switchboard-dds-requesthandler.h++
/// @brief Handle Switchboard RPC requests
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "switchboard-types.hpp"      // generated from `switchboard-types.idl`
#include "switchboard-interface.hpp"  // generated from `switchboard-service.idl`
#include "translate-idl-switchboard.h++"
#include "switchboard-provider.h++"
#include "types/create-shared.h++"

namespace switchboard::dds
{
    //==========================================================================
    // @class RequestHandler
    // @brief Handle requests from and publish updates to Switchboard clients

    class RequestHandler : public CC::Switchboard::Switchboard,
                           public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience alias
        using Super = CC::Switchboard::Switchboard;

    public:
        RequestHandler(const std::shared_ptr<Provider> &provider);

    public:
        CC::Switchboard::SwitchList get_switches() override;
        CC::Switchboard::Switch get_switch(const SwitchName &switch_name) override;
        bool add_switch(const CC::Switchboard::AddSwitchRequest &req) override;
        bool remove_switch(const CC::Switchboard::RemoveSwitchRequest &req) override;
        std::uint32_t import_switches(const CC::Variant::ValueList &req) override;
        bool set_specification(const CC::Switchboard::SetSpecificationRequest &req) override;

        bool add_dependency(const CC::Switchboard::AddDependencyRequest &req) override;
        bool remove_dependency(const CC::Switchboard::RemoveDependencyRequest &req) override;

        bool add_interceptor(const CC::Switchboard::AddInterceptorRequest &req) override;
        bool remove_interceptor(const CC::Switchboard::RemoveInterceptorRequest &req) override;
        CC::Switchboard::InterceptorResult invoke_interceptor(const CC::Switchboard::InterceptorInvocation &req) override;

        bool set_target(const CC::Switchboard::SetTargetRequest &req) override;
        bool set_attributes(const CC::Switchboard::SetAttributesRequest &req) override;
        CC::Switchboard::StatusList get_culprits(const CC::Switchboard::CulpritsQuery &req) override;

    private:
        std::shared_ptr<Provider> provider;
    };
}  // namespace switchboard
