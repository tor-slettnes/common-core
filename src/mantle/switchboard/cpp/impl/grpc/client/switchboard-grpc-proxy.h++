// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-proxy.h++
/// @brief Switchboard provider supporting local and remote switches
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "switchboard.grpc.pb.h"  // Generated from `switchboard.proto`
#include "switchboard.h++"
#include "grpc-signalclient.h++"
#include "string/misc.h++"

namespace switchboard::grpc
{
    class Proxy : public switchboard::Provider,
                  public core::grpc::SignalClient<cc::switchboard::Switchboard, cc::switchboard::Signal>,
                  public core::types::enable_create_shared<Proxy>
    {
        using This = Proxy;
        using Super = switchboard::Provider;

    protected:
        Proxy(const std::string &host,
              bool wait_for_ready = false,
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
            const SwitchName &switch_name) override;

        bool remove_switch(
            const SwitchName &switch_name,
            bool propagate = true) override;

    private:
        void synchronize_switches();

        void on_spec_update(
            core::signal::MappingAction action,
            const std::string &switch_name,
            const cc::switchboard::Signal &signal);

        void on_status_update(
            core::signal::MappingAction action,
            const std::string &switch_name,
            const cc::switchboard::Signal &signal);

    protected:
        core::dt::Duration ready_timeout;
    };
};  // namespace switchboard::grpc
