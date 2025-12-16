// -*- c++ -*-
//==============================================================================
/// @file switchboard-central.h++
/// @brief Switchboard provider supporting only local switches
/// @author Tor Slettnes <tor@slett.net>
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
            const SwitchName &switch_name) override;

    private:
        core::SettingsStore settings_;

    };
};  // namespace switchboard
