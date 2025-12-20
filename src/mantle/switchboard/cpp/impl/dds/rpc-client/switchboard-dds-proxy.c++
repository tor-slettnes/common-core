// -*- c++ -*-
//==============================================================================
/// @file switchboard-dds-proxy.c++
/// @brief Switchboard provider supporting remote switches
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "switchboard-dds-proxy.h++"
#include "switchboard-dds-subscriber.h++"
#include "remote-dds-switch.h++"
#include "translate-idl-variant.h++"
#include "translate-idl-inline.h++"
#include "platform/symbols.h++"

#include <memory>

namespace switchboard::dds
{
    Proxy::Proxy(int domain_id, const core::dt::Duration &ready_timeout)
        : Provider(TYPE_NAME_FULL(This)),
          ClientWrapper(CC::Switchboard::SERVICE_INTERFACE_ID, domain_id),
          ready_timeout_(ready_timeout),
          synchronized_(false)
    {
    }

    void Proxy::initialize()
    {
        Provider::initialize();

        using namespace std::placeholders;
        switchboard::dds::signal_dds_spec.connect(
            this->client()->to_string(),
            std::bind(&Proxy::on_spec_update, this, _1, _2, _3));

        switchboard::dds::signal_dds_status.connect(
            this->client()->to_string(),
            std::bind(&Proxy::on_status_update, this, _1, _2, _3));

        // core::platform::signal_shutdown.connect(
        //     this->name(),
        //     std::bind(&Proxy::deinitialize, this));

        // this->synchronize_switches();
    }

    void Proxy::deinitialize()
    {
        // core::platform::signal_shutdown.disconnect(this->name());
        switchboard::dds::signal_dds_status.disconnect(
            this->client()->to_string());

        switchboard::dds::signal_dds_spec.disconnect(
            this->client()->to_string());

        Provider::deinitialize();
    }

    bool Proxy::available() const
    {
        return this->wait_for_service(core::dt::Duration::zero());
    }

    bool Proxy::wait_ready() const
    {
        return this->wait_for_service(this->ready_timeout_);
    }

    SwitchMap Proxy::get_switches() const
    {
        const_cast<Proxy *>(this)->synchronize_switches();
        return Provider::get_switches();
    }

    SwitchRef Proxy::get_switch(
        const SwitchName &name,
        bool required) const
    {
        const_cast<Proxy *>(this)->synchronize_switches();
        return Provider::get_switch(name, required);
    }

    std::pair<SwitchRef, bool> Proxy::add_switch(const SwitchName &name)
    {
        auto [sw, inserted] = this->find_or_insert<RemoteSwitch>(
            name,
            this->shared_from_this());

        if (inserted)
        {
            this->client()->add_switch(CC::Switchboard::AddSwitchRequest(name));
        }
        return {sw, inserted};
    }

    bool Proxy::remove_switch(
        const SwitchName &name,
        bool propagate)
    {
        return this->client()->remove_switch({name, propagate});
    }

    uint Proxy::import_switches(
        const core::types::ValueList &switches)
    {
        return this->client()->import_switches(
            idl::encoded<CC::Variant::ValueList>(switches));
    }

    bool Proxy::wait_for_service(
        const core::dt::Duration &timeout) const
    {
        try
        {
            this->client()->wait_for_service(timeout);
            return true;
        }
        catch (const std::runtime_error &)
        {
            return false;
        }
    }

    void Proxy::synchronize_switches()
    {
        if (!this->synchronized_)
        {
            logf_debug("Reading switches from server");
            for (const CC::Switchboard::Switch &data : this->client()->get_switches().list())
            {
                this->on_spec_update(
                    core::signal::MAP_ADDITION,
                    data.spec().switch_name(),
                    data.spec());

                this->on_status_update(
                    core::signal::MAP_ADDITION,
                    data.spec().switch_name(),
                    data.status());
            }
            this->synchronized_ = true;
            logf_debug("Switches are now synchronized");
        }
    }

    void Proxy::on_spec_update(
        core::signal::MappingAction action,
        const std::string &switch_name,
        const CC::Switchboard::Specification &spec)
    {
        logf_trace("Received spec %s: %s", action, spec);
        if (SwitchRef sw = this->sync_switch<RemoteSwitch>(action, switch_name))
        {
            idl::decode(spec, this->shared_from_this(), nullptr, sw->spec().get());
            switchboard::signal_spec.emit(action, switch_name, *sw->spec());
        }
        else
        {
            switchboard::signal_spec.clear(switch_name);
        }
    }

    void Proxy::on_status_update(
        core::signal::MappingAction action,
        const std::string &switch_name,
        const CC::Switchboard::Status &status)
    {
        logf_trace("Received spec %s: %s", action, status);
        if (SwitchRef sw = this->sync_switch<RemoteSwitch>(action, switch_name))
        {
            idl::decode(status, nullptr, sw->status().get());
            switchboard::signal_status.emit(action, switch_name, *sw->status());
        }
        else
        {
            switchboard::signal_status.clear(switch_name);
        }
    }
}  // namespace switchboard::dds
