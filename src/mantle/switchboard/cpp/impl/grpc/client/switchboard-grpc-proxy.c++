// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-proxy.c++
/// @brief Switchboard provider supporting local and remote switches
/// @author Tor Slettnes
//==============================================================================

#include "switchboard-grpc-proxy.h++"
#include "remote-grpc-switch.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-switchboard-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"

namespace cc::platform::switchboard::grpc
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
        Super::initialize();
        SignalClient::initialize();

        using namespace std::placeholders;

        this->add_mapping_handler(
            cc::platform::switchboard::protobuf::Signal::kSpecification,
            std::bind(&This::on_spec_update, this, _1, _2, _3));

        this->add_mapping_handler(
            cc::platform::switchboard::protobuf::Signal::kStatus,
            std::bind(&This::on_status_update, this, _1, _2, _3));

        this->start_watching();
    }

    void Proxy::deinitialize()
    {
        this->stop_watching();
        SignalClient::deinitialize();
        Super::deinitialize();
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
        return Super::get_switches();
    }

    SwitchRef Proxy::get_switch(
        const SwitchName &name,
        bool required) const
    {
        this->wait_ready();
        return Super::get_switch(name, required);
    }

    std::pair<SwitchRef, bool> Proxy::add_switch(
        const SwitchName &switch_name,
        bool active)
    {
        auto [sw, inserted] = this->find_or_insert<RemoteSwitch>(
            switch_name,
            this->shared_from_this());

        if (inserted)
        {
            sw->status()->active = active;

            cc::platform::switchboard::protobuf::AddSwitchRequest req;
            req.set_switch_name(switch_name);
            req.set_active(active);
            this->call_check(&Stub::AddSwitch, req);
        }

        return {sw, inserted};
    }

    bool Proxy::remove_switch(
        const SwitchName &switch_name,
        bool propagate)
    {
        cc::platform::switchboard::protobuf::RemoveSwitchRequest req;
        req.set_switch_name(switch_name);
        req.set_propagate(propagate);
        return this->call_check(&Stub::RemoveSwitch, req).value();
    }

    bool Proxy::clear_switches(
        bool reload)
    {
        cc::platform::switchboard::protobuf::ClearSwitchesRequest req;
        req.set_reload(reload);
        return this->call_check(&Stub::ClearSwitches, req).value();
    }

    uint Proxy::import_switches(
        const core::types::KeyValueMap &declarations,
        bool replace_specifications,
        bool replace_statuses,
        InvocationStyle invoke_interceptors)
    {
        cc::platform::switchboard::protobuf::ImportRequest req;
        cc::protobuf::encode(declarations, req.mutable_declarations());
        req.set_replace_specifications(replace_specifications);
        req.set_replace_statuses(replace_statuses);
        req.set_invoke_interceptors(
            cc::protobuf::encoded<cc::platform::switchboard::protobuf::InvocationStyle>(
                invoke_interceptors));
        return this->call_check(&Stub::ImportSwitches, req).import_count();
    }

    core::types::KeyValueMap Proxy::export_switches(
        const std::optional<SwitchSelection> &selection,
        bool include_specifications,
        bool include_statuses) const
    {
        cc::platform::switchboard::protobuf::ExportRequest req;
        if (selection)
        {
            cc::protobuf::encode(*selection, req.mutable_selection());
        }
        req.set_include_specifications(include_specifications);
        req.set_include_statuses(include_statuses);
        return cc::protobuf::decoded<core::types::KeyValueMap>(
            this->call_check(&Stub::ExportSwitches, req).declarations());
    }

    bool Proxy::add_interceptor(
        const InterceptorRef &interceptor,
        const SwitchSelection &switch_selection,
        bool immediate,
        bool future)
    {
        cc::platform::switchboard::protobuf::AddInterceptorRequest req;
        cc::protobuf::encode(interceptor, req.mutable_spec());
        cc::protobuf::encode(switch_selection, req.mutable_switch_selection());
        req.set_immediate(immediate);
        req.set_future(future);
        return this->call_check(&Stub::AddInterceptor, req).value();
    }

    bool Proxy::remove_interceptor(
        const InterceptorName &name,
        const std::optional<SwitchSelection> &switch_selection)
    {
        cc::platform::switchboard::protobuf::RemoveInterceptorRequest req;
        req.set_interceptor_name(name);
        if (switch_selection.has_value())
        {
            cc::protobuf::encode(*switch_selection, req.mutable_switch_selection());
        }
        return this->call_check(&Stub::RemoveInterceptor, req).value();
    }

    void Proxy::on_spec_update(
        core::signal::MappingAction action,
        const std::string &switch_name,
        const cc::platform::switchboard::protobuf::Signal &signal)
    {
        if (!switch_name.empty())
        {
            if (SwitchRef sw = this->sync_switch<RemoteSwitch>(action, switch_name))
            {
                cc::protobuf::decode(signal.specification(),
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
        const cc::platform::switchboard::protobuf::Signal &signal)
    {
        if (!switch_name.empty())
        {
            if (SwitchRef sw = this->sync_switch<RemoteSwitch>(action, switch_name))
            {
                cc::protobuf::decode(signal.status(), sw->status().get());
                switchboard::signal_status.emit(action, switch_name, *sw->status());
            }
            else
            {
                switchboard::signal_status.clear(switch_name);
            }
        }
    }

};  // namespace cc::platform::switchboard::grpc
