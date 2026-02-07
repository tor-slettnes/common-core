// -*- c++ -*-
//==============================================================================
/// @file switchboard-central.h++
/// @brief Switchboard provider supporting only local switches
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "switchboard.h++"
#include "types/create-shared.h++"
#include "settings/settingsstore.h++"

namespace switchboard
{
    //==========================================================================
    /// @class Central
    /// @brief Authoritative/central switchboard provider
    ///
    /// This implements a collection of authoritative @sa LocalSwitch instances,
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

    protected:
        Central();

    public:
        void initialize() override;
        bool available() const override;
        bool wait_ready() const override;

        std::pair<SwitchRef, bool> add_switch(
            const SwitchName &switch_name,
            bool active = false) override;

        uint import_switches(
            const core::types::KeyValueMap &switches,
            bool replace_specifications,
            bool replace_statuses) override;

        core::types::KeyValueMap export_switches(
            const std::optional<SwitchSelection> &selection,
            bool include_specifications,
            bool include_statuses) const override;

    private:
        void import_switch(
            const std::string &name,
            const core::types::KeyValueMap &declaration,
            bool replace_specification,
            bool replace_status);

        static void import_spec(
            const SwitchRef &sw,
            const core::types::KeyValueMap &kvmap);

        static Localization import_localization(
            const core::types::KeyValueMap &kvmap);

        static DependencyRef import_dependency(
            const SwitchRef &sw,
            const std::string &predecessor_name,
            const core::types::KeyValueMap &dep_map);

        static void import_status(
            const SwitchRef &sw,
            const core::types::KeyValueMap &status);

        SwitchMap find_regex_matches(
            const std::vector<std::string> &patterns) const;

        SwitchMap find_glob_matches(
            const std::vector<std::string> &patterns) const;

        static core::types::KeyValueMap export_switch(
            const SwitchRef &sw,
            bool include_specification,
            bool include_status);

        static void export_status(
            const SwitchRef &sw,
            core::types::TaggedValueList *tvlist);
    };
};  // namespace switchboard
