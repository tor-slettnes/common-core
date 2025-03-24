/// -*- c++ -*-
//==============================================================================
/// @file nm-device-wireless.c++
/// @brief DBus Proxy for Network Manager Device.WIreless objects
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "nm-device.h++"
#include "status/exceptions.h++"

namespace netconfig::dbus
{
    WirelessDevice::WirelessDevice(
        core::dbus::ProxyContainer *container,
        const core::dbus::ConnectionPtr &connection,
        const core::dbus::ServiceName &servicename,
        const core::dbus::ObjectPath &objectpath)
        : DataWrapper<WirelessDeviceData>(
              container,
              connection,
              servicename,
              objectpath,
              NM_DBUS_INTERFACE_DEVICE_WIRELESS,
              {
                  {"AccessPointAdded",   SLOT(Class::on_signal_accesspoint_added)  },
                  {"AccessPointRemoved", SLOT(Class::on_signal_accesspoint_removed)},
    },
              {
                  {"Mode", DATASLOT(&this->mode)},
                  {"Bitrate", DATASLOT(&this->bitrate)},
                  {"ActiveAccessPoint", SLOT(Class::on_property_active_accesspoint)},
                  {"LastScan", SLOT(Class::on_property_lastscan)},
                  {"PermHwAddress", SLOT(Class::on_property_hwaddress)},
                  {"ActiveConnection", SLOT(Class::on_property_active_connection)},
              })
    {
    }

    void WirelessDevice::initialize()
    {
        this->initialize_properties();
        auto paths = this->get_cached_property<core::dbus::ObjectPaths>("AccessPoints");
        this->container->synchronize<AccessPoint>(paths);
        this->accesspoints = std::set(paths.begin(), paths.end());
        this->set_ready();
    }

    void WirelessDevice::emit_change(core::signal::MappingAction action)
    {
        if (auto device = this->container->get<Device>(this->objectpath))
        {
            device->update_specific_data(this, action);
        }
    }

    std::shared_ptr<WirelessDevice> WirelessDevice::get_by_ap(
        std::shared_ptr<AccessPoint> ap,
        bool required)
    {
        for (const auto &[path, wifidev] : dbus::container.instances<WirelessDevice>())
        {
            if (auto dev = dbus::container.get<Device>(path))
            {
                if (wifidev->accesspoints.count(ap->objectpath))
                {
                    logf_trace("Found device for AP %r: %r", ap->key(), dev->key());
                    return wifidev;
                }
            }
        }
        if (required)
        {
            assertf(ap, "Cannot require access point by empty reference");

            throwf_args(core::exception::NotFound,
                        ("Access point %r not found on any wireless device", ap->key()),
                        ap->key());
        }
        return {};
    }

    std::shared_ptr<WirelessDevice> WirelessDevice::first()
    {
        std::shared_ptr<WirelessDevice> dev;
        for (const auto &[path, ref] : dbus::container.instances<WirelessDevice>())
        {
            if (auto dev = dbus::container.get<Device>(path))
            {
                if (dev->is_managed())
                {
                    return ref;
                }
            }
        }
        throw core::exception::NotFound("No wireless device");
    }

    void WirelessDevice::on_signal_accesspoint_added(
        const Glib::VariantContainerBase &parameters)
    {
        logf_trace("on_signal_accesspoint_added: %s", parameters);
        auto path = core::glib::variant_cast<core::dbus::ObjectPath>(parameters, 0);
        if (this->valid_path(path))
        {
            auto ref = this->container->add<AccessPoint>(path);
            this->accesspoints.insert(path);
        }
    }

    void WirelessDevice::on_signal_accesspoint_removed(
        const Glib::VariantContainerBase &parameters)
    {
        logf_trace("on_signal_accesspoint_removed: %s", parameters);
        auto path = core::glib::variant_cast<core::dbus::ObjectPath>(parameters, 0);
        this->accesspoints.erase(path);
        this->container->remove(path);
    }

    void WirelessDevice::on_property_active_accesspoint(
        const Glib::VariantBase &change)
    {
        auto path = core::glib::variant_cast<core::dbus::ObjectPath>(change);
        if (this->valid_path(path))
        {
            auto ref = this->container->add<AccessPoint>(path);
            ref->subscribe_updates(this, SLOT(Class::update_active_accesspoint));
        }
        else
        {
            this->active_accesspoint.clear();
        }
    }

    void WirelessDevice::on_property_lastscan(
        const Glib::VariantBase &change)
    {
        auto millisecs = core::glib::variant_cast<std::int64_t>(change);
        std::chrono::duration uptime = core::steady::Clock::now().time_since_epoch();
        this->lastScan = core::dt::Clock::now() - uptime + std::chrono::milliseconds(millisecs);
    }

    void WirelessDevice::on_property_hwaddress(
        const Glib::VariantBase &change)
    {
        // In NetworkManager v1.24 and newer,'HwAddress' is a property of
        // Device, not WirelessDevice. We emulate the newer behavior by
        // assigning it there.
        if (auto device = this->container->get<Device>(this->objectpath))
        {
            core::glib::variant_cast(change, &device->hwAddress);
        }
    }

    void WirelessDevice::on_property_active_connection(
        const Glib::VariantBase &change)
    {
        // Per NetworkManager DBus documentation, 'ActiveConnection' is a
        // property on ...Device, not ...Device.Wireless, but in practice
        // that does not seem to be the case. So if we receive changes here,
        // pass them on to the underlying Device.
        if (auto device = this->container->get<Device>(this->objectpath))
        {
            device->on_property_active_connection(change);
        }
    }

    bool WirelessDevice::update_active_accesspoint(
        const core::dbus::ProxyWrapper *source,
        core::signal::MappingAction action)
    {
        if (const auto *datasource = dynamic_cast<const AccessPoint *>(source))
        {
            switch (action)
            {
            case core::signal::MAP_ADDITION:
            case core::signal::MAP_UPDATE:
                this->active_accesspoint = datasource->key();
                break;

            case core::signal::MAP_REMOVAL:
                this->active_accesspoint.clear();
                break;

            default:
                break;
            }
            this->emit_change(action);
        }
        return false; // No need for further updates from this AP
    }

    void WirelessDevice::request_scan() const
    {
        if (auto dev = this->container->get<Device>(this->objectpath))
        {
            if (dev->is_available())
            {
                GVariantBuilder builder;
                g_variant_builder_init(&builder, G_VARIANT_TYPE("a{sv}"));
                Glib::VariantBase options = Glib::wrap(g_variant_builder_end(&builder));
                auto inputs = Glib::VariantContainerBase::create_tuple({options});
                this->call_sync("RequestScan", inputs);
                logf_debug("Requested scan on %s (%s)", this->identifier(), dev->key());
            }
        }
    }
} // namespace netconfig::dbus
