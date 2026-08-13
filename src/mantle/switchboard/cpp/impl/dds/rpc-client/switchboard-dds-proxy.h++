// -*- c++ -*-
//==============================================================================
/// @file switchboard-dds-proxy.h++
/// @brief Switchboard provider supporting remote switches
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "switchboard-interface.hpp"  // generated from `switchboard-interface.idl`
#include "switchboard.h++"
#include "translate-idl-switchboard.h++"
#include "dds-client.h++"
#include "chrono/date-time.h++"
#include "types/create-shared.h++"

namespace cc::platform::switchboard::dds
{
    //==========================================================================
    /// @class Proxy
    /// @brief Switchboard provider using replicated @sa RemoteSwitch instances,
    ///
    /// This implements a remote replica/shadow of @sa switcboard::Central.
    /// Updates from the central server are received via DDS publications,
    /// and any local changes are propagated back to the Central over RPC.

    class Proxy : public switchboard::Provider,
                  public cc::dds::ClientWrapper<CC::Switchboard::SwitchboardClient>,
                  public core::types::enable_create_shared<Proxy>
    {
        using This = Proxy;
        using Super = switchboard::Provider;

    protected:
        Proxy(int domain_id,
              const core::dt::Duration &ready_timeout = std::chrono::seconds(10));

    public:
        void initialize() override;
        void deinitialize() override;

        bool available() const override;
        bool wait_ready() const override;
        SwitchMap get_switches() const override;

        SwitchRef get_switch(
            const SwitchName &name,
            bool required) const override;

        std::pair<SwitchRef, bool> add_switch(
            const SwitchName &switch_name,
            bool active = false) override;

        bool remove_switch(
            const SwitchName &switch_name,
            bool propagate = true) override;

        bool clear_switches(
            bool reload = false) override;

        uint import_switches(
            const core::types::KeyValueMap &declarations,
            bool replace_specifications,
            bool replace_statuses,
            InvocationStyle invoke_interceptors) override;

        core::types::KeyValueMap export_switches(
            const std::optional<SwitchSelection> &selection,
            bool include_specifications,
            bool include_statuses) const override;

        bool add_interceptor(
            const InterceptorRef &interceptor,
            const SwitchSelection &switch_selection,
            bool immediate = false,
            bool future = true) override;

        bool remove_interceptor(
            const InterceptorName &name,
            const std::optional<SwitchSelection> &switch_selection = {}) override;

    private:
        bool wait_for_service(const core::dt::Duration &timeout) const;

        void synchronize_switches();

        void on_spec_update(core::signal::MappingAction action,
                            const std::string &switch_name,
                            const CC::Switchboard::Specification &spec);

        void on_status_update(core::signal::MappingAction action,
                              const std::string &switch_name,
                              const CC::Switchboard::Status &status);

    private:
        core::dt::Duration ready_timeout_;
        bool synchronized_;
    };
};  // namespace cc::platform::switchboard::dds
