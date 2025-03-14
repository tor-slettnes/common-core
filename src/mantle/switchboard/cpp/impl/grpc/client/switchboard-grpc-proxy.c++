// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-proxy.c++
/// @brief Switchboard provider supporting local and remote switches
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "switchboard-grpc-proxy.h++"
#include "remote-grpc-switch.h++"
#include "protobuf-switchboard-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"

namespace switchboard::grpc
{
    Proxy::Proxy(
        const std::string &host,
        bool wait_for_ready,
        const core::dt::Duration &ready_timeout)
        : Provider(TYPE_NAME_FULL(This)),
          SignalClient(host, wait_for_ready),
          ready_timeout(ready_timeout)
    {
    }

    void Proxy::initialize()
    {
        Provider::initialize();
        SignalClient::initialize();

        using namespace std::placeholders;

        this->add_mapping_handler(
            cc::switchboard::Signal::kSpecification,
            std::bind(&This::on_spec_update, this, _1, _2, _3));

        this->add_mapping_handler(
            cc::switchboard::Signal::kStatus,
            std::bind(&This::on_status_update, this, _1, _2, _3));

        this->start_watching();
    }

    void Proxy::deinitialize()
    {
        this->stop_watching();
        SignalClient::deinitialize();
        Provider::deinitialize();
    }

    bool Proxy::available() const
    {
        return this->is_complete();
    }

    bool Proxy::wait_ready() const
    {
        return this->wait_complete(this->ready_timeout);
    }

    SwitchMap Proxy::get_switches() const
    {
        this->wait_ready();
        return Provider::get_switches();
    }

    SwitchRef Proxy::get_switch(
        const SwitchName &name,
        bool required) const
    {
        this->wait_ready();
        return Provider::get_switch(name, required);
    }

    std::pair<SwitchRef, bool> Proxy::add_switch(
        const SwitchName &switch_name)
    {
        auto [sw, inserted] = this->find_or_insert<RemoteSwitch>(
            switch_name,
            this->shared_from_this());

        if (inserted)
        {
            cc::switchboard::AddSwitchRequest req;
            req.set_switch_name(switch_name);
            this->call_check(&Stub::add_switch, req);
        }

        return {sw, inserted};
    }

    bool Proxy::remove_switch(
        const SwitchName &switch_name,
        bool propagate)
    {
        cc::switchboard::RemoveSwitchRequest req;
        req.set_switch_name(switch_name);
        req.set_propagate(propagate);
        return this->call_check(&Stub::remove_switch, req).value();
    }

    void Proxy::on_spec_update(
        core::signal::MappingAction action,
        const std::string &switch_name,
        const cc::switchboard::Signal &signal)
    {
        if (!switch_name.empty())
        {
            if (SwitchRef sw = this->sync_switch<RemoteSwitch>(action, switch_name))
            {
                protobuf::decode(signal.specification(),
                                 this->shared_from_this(),
                                 sw->spec().get());

                switchboard::signal_spec.emit(action, switch_name, *sw->spec());
            }
            else
            {
                switchboard::signal_spec.clear(switch_name);
            }
        }
    }

    void Proxy::on_status_update(
        core::signal::MappingAction action,
        const std::string &switch_name,
        const cc::switchboard::Signal &signal)
    {
        if (!switch_name.empty())
        {
            if (SwitchRef sw = this->sync_switch<RemoteSwitch>(action, switch_name))
            {
                protobuf::decode(signal.status(), sw->status().get());
                switchboard::signal_status.emit(action, switch_name, *sw->status());
            }
            else
            {
                switchboard::signal_status.clear(switch_name);
            }
        }
    }
};  // namespace switchboard::grpc
