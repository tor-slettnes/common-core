/// -*- c++ -*-
//==============================================================================
/// @file nm-device.c++
/// @brief DBus Proxy for Network Manager Device objects
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "nm-device.h++"
#include "nm-ipconfig.h++"
#include "nm-connection-active.h++"
#include "netconfig.h++"
#include "status/exceptions.h++"

namespace platform::netconfig::dbus
{
    //==========================================================================
    /// Network Manager device

    Device::Device(
        core::dbus::ProxyContainer *container,
        const core::dbus::ConnectionPtr &connection,
        const core::dbus::ServiceName &servicename,
        const core::dbus::ObjectPath &objectpath)
        : MappedDataWrapper<DeviceData>(
              container,
              connection,
              servicename,
              objectpath,
              NM_DBUS_INTERFACE_DEVICE,
              {
                  {"StateChanged", SLOT(Class::on_signal_state_changed)},
    },
              {
                  {"DeviceType", DATASLOT(&this->type)},
                  {"Ip4Connectivity", DATASLOT(&this->ip4connectivity)},
                  {"Ip6Connectivity", DATASLOT(&this->ip6connectivity)},
                  {"InterfaceFlags", DATASLOT(&this->flags)},
                  {"Interface", DATASLOT(&this->interface)},
                  {"HwAddress", DATASLOT(&this->hwAddress)},
                  {"State", DATASLOT(&this->state)},
                  {"Ip4Config", SLOT(Device::on_property_ip4config)},
                  {"Ip6Config", SLOT(Device::on_property_ip6config)},
                  {"ActiveConnection", SLOT(Device::on_property_active_connection)},
              })
    {
    }

    void Device::initialize()
    {
        this->initialize_properties();
        switch (this->type)
        {
        case NM_DEVICE_TYPE_ETHERNET:
            this->container
                ->add<WiredDevice>(this->objectpath)
                ->subscribe_updates(this, SLOT(Device::add_specific_data));
            break;

        case NM_DEVICE_TYPE_WIFI:
            this->container
                ->add<WirelessDevice>(this->objectpath)
                ->subscribe_updates(this, SLOT(Device::add_specific_data));
            break;

        default:
            logf_trace("%s ignoring device type: %d", this->identifier(), this->type);
            this->set_ready();
            break;
        }
    }

    void Device::emit_change(core::signal::MappingAction action)
    {
        if (this->ready)
        {
            logf_trace("signal_device: %s", *this);
            auto ref = std::static_pointer_cast<Device>(shared_from_this());
            signal_device.emit(action, this->key(), ref);
            Super::emit_change(action);
        }
    }

    void Device::on_signal_state_changed(
        const Glib::VariantContainerBase &parameters)
    {
        core::glib::variant_cast(parameters, 0, &this->state);
        core::glib::variant_cast(parameters, 2, &this->state_reason);
        this->emit_change(core::signal::MAP_UPDATE);
    }

    void Device::on_property_ip4config(const Glib::VariantBase &change)
    {
        auto path = core::glib::variant_cast<core::dbus::ObjectPath>(change);
        if (this->valid_path(path))
        {
            auto ref = this->container->add<IP4Config>(path);
            ref->subscribe_updates(this, SLOT(Class::update_ip4config));
        }
    }

    void Device::on_property_ip6config(const Glib::VariantBase &change)
    {
        auto path = core::glib::variant_cast<core::dbus::ObjectPath>(change);
        if (this->valid_path(path))
        {
            auto ref = this->container->add<IP6Config>(path);
            ref->subscribe_updates(this, SLOT(Class::update_ip6config));
        }
    }

    void Device::on_property_active_connection(const Glib::VariantBase &change)
    {
        auto path = core::glib::variant_cast<core::dbus::ObjectPath>(change);
        if (this->valid_path(path))
        {
            logf_trace("Device %s setting active connection %s", this->identifier(), path);
            auto ref = this->container->add<ActiveConnection>(path);
            ref->subscribe_updates(this, SLOT(Class::update_active_connection));
        }
    }

    bool Device::update_ip4config(const core::dbus::ProxyWrapper *source,
                                  core::signal::MappingAction action)
    {
        if (const auto *datasource = dynamic_cast<const IP4Config *>(source))
        {
            this->ip4config = *datasource;
            logf_trace("%s updated IP4 config: %s", this->identifier(), *datasource);
            this->emit_change(action);
        }
        return true; // Stay subscribed to updates
    }

    bool Device::update_ip6config(const core::dbus::ProxyWrapper *source,
                                  core::signal::MappingAction action)
    {
        if (const auto *datasource = dynamic_cast<const IP6Config *>(source))
        {
            this->ip6config = *datasource;
            logf_trace("%s updated IP6 config: %s", this->identifier(), *datasource);
            this->emit_change(action);
        }
        return true; // Stay subscrbed to updates
    }

    bool Device::update_active_connection(
        const core::dbus::ProxyWrapper *source,
        core::signal::MappingAction action)
    {
        if (const auto *datasource = dynamic_cast<const ActiveConnection *>(source))
        {
            logf_debug("Device %s active connection: %r",
                       this->identifier(),
                       datasource->key());

            switch (action)
            {
            case core::signal::MAP_ADDITION:
            case core::signal::MAP_UPDATE:
                this->active_connection = datasource->key();
                break;

            case core::signal::MAP_REMOVAL:
                this->active_connection.clear();
                break;

            default:
                break;
            }

            this->emit_change(core::signal::MAP_UPDATE);
        }
        return false; // No need for further updates from this AC
    }

    bool Device::add_specific_data(
        const core::dbus::ProxyWrapper *source,
        core::signal::MappingAction action)
    {
        this->update_specific_data(source, action);
        this->set_ready();
        return false;
    }

    void Device::update_specific_data(
        const core::dbus::ProxyWrapper *source,
        core::signal::MappingAction action)
    {
        if (const auto *datasource = dynamic_cast<const WiredDevice *>(source))
        {
            this->specific_data = *datasource;
        }
        else if (const auto *datasource = dynamic_cast<const WirelessDevice *>(source))
        {
            this->specific_data = *datasource;
        }
        this->emit_change(core::signal::MAP_UPDATE);
        logf_trace("%s specific data update from %s: %s",
                   this->identifier(),
                   source->identifier(),
                   this->specific_data);
    }

    std::shared_ptr<Device> Device::get_by_interface(
        const Key &ifkey,
        bool required)
    {
        for (const auto &[path, ref] : dbus::container.instances<Device>())
        {
            auto ifname = ref->get_cached_property<std::string>("Interface");
            if (ifname == ifkey)
            {
                logf_trace("Found device forinterface %r, path=%s", ifname, path);
                return ref;
            }
        }

        if (required)
        {
            throwf_args(core::exception::NotFound,
                        ("Interface name not found: %s", ifkey),
                        ifkey);
        }

        return {};
    }

    std::shared_ptr<Device> Device::get_by_active_connection(
        const Key &ackey,
        bool required)
    {
        for (const auto &[path, ref] : dbus::container.instances<Device>())
        {
            if (ref->active_connection == ackey)
            {
                return ref;
            }
        }

        if (required)
        {
            throwf_args(core::exception::NotFound,
                        ("Active connection not found: %s", ackey),
                        ackey);
        }

        return {};
    }

    bool Device::is_available()
    {
        return (this->state != NM_DEVICE_STATE_UNKNOWN) &&
               (this->state != NM_DEVICE_STATE_UNMANAGED);
    }

    void Device::disconnect()
    {
        logf_info("Disconnecting device %s (%s)", this->identifier(), this->key());
        this->call_sync("Disconnect");
    }
} // namespace platform::netconfig::dbus
