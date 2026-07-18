// -*- c++ -*-
//==============================================================================
/// @file switchboard-central.h++
/// @brief Switchboard provider supporting only local switches
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "switchboard.h++"
#include "central-switch.h++"
#include "types/create-shared.h++"
#include "settings/settingsstore.h++"

namespace cc::platform::switchboard
{
    //==========================================================================
    /// @class Central
    /// @brief Authoritative/central switchboard provider
    ///
    /// This implements a collection of authoritative @sa CentralSwitch instances,
    /// either loaded from a configuration file at startup or added later.
    ///
    /// Most likely there will be only one `Central` instance in a deployed
    /// system, paired with a DDS switchboard service. Other (client)
    /// applications can then use @sa switchboard::Relay to replicate its
    /// switches as @sa switchboard::RemoteSwitch instances.

    class Central : public Provider,
                    public core::types::enable_create_shared<Central>
    {
        using This = Central;
        using Super = Provider;

        struct InterceptorFactoryData
        {
            InterceptorRef interceptor;
            SwitchSelection switch_selection;
            bool immediate = false;
        };

        using InterceptorFactoryMap = std::unordered_map<
            InterceptorName,
            InterceptorFactoryData>;

        using CentralSwitchRef = std::shared_ptr<CentralSwitch>;

    protected:
        Central();

    public:
        void initialize() override;
        bool available() const override;
        bool wait_ready() const override;

        std::pair<SwitchRef, bool> add_switch(
            const SwitchName &switch_name,
            bool active = false) override;

        bool remove_switch(
            const SwitchName &switch_name,
            bool propagate = true) override;

        bool clear_switches(
            bool reload = false) override;

        uint import_switches(
            const core::types::KeyValueMap &switches,
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
        std::pair<CentralSwitchRef, bool> insert_switch(
            const SwitchName &switch_name);

        void load_default_switches(
            bool replace_specifications = false,
            bool replace_statuses = false,
            InvocationStyle invoke_interceptors = InvocationStyle::INDIRECT);

        bool import_switch(
            const std::string &name,
            const core::types::KeyValueMap &declaration,
            bool replace_specification,
            bool replace_status,
            InvocationStyle invoke_interceptors);

        core::types::KeyValueMap export_switch(
            const SwitchRef &sw,
            bool include_specification,
            bool include_status) const;

    private:
        InterceptorFactoryMap interceptor_factory_map;
    };
};  // namespace cc::platform::switchboard
