/// -*- c++ -*-
//==============================================================================
/// @file nm-device-wired.c++
/// @brief DBus Proxy for Network Manager Device.Wired objects
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "nm-device.h++"
#include "status/exceptions.h++"

namespace platform::netconfig::dbus
{
    WiredDevice::WiredDevice(
        core::dbus::ProxyContainer* container,
        const core::dbus::ConnectionRef& connection,
        const core::dbus::ServiceName& servicename,
        const core::dbus::ObjectPath& objectpath)
        : DataWrapper<WiredDeviceData>(
              container,
              connection,
              servicename,
              objectpath,
              NM_DBUS_INTERFACE_DEVICE_WIRED,
              {},
              {
                  {"Speed", DATASLOT(&this->speed)},
                  {"HwAddress", SLOT(Class::on_property_hwaddress)},
                  {"ActiveConnection", SLOT(Class::on_property_active_connection)},
              })
    {
    }

    void WiredDevice::emit_change(core::signal::MappingAction action)
    {
        if (auto device = this->container->get<Device>(this->objectpath))
        {
            device->update_specific_data(this, action);
        }
    }

    std::shared_ptr<WiredDevice> WiredDevice::first()
    {
        for (const auto& [path, ref] : dbus::container.instances<WiredDevice>())
        {
            if (auto dev = dbus::container.get<Device>(path))
            {
                if (dev->is_managed())
                {
                    return ref;
                }
            }
        }
        throw core::exception::NotFound("No wired device");
    }

    void WiredDevice::on_property_hwaddress(
        const Glib::VariantBase& change)
    {
        // In NetworkManager v1.24 and newer,'HwAddress' is a property of
        // Device, not WiredDevice. We emulate the newer behavior by
        // assigning it there.
        if (auto device = this->container->get<Device>(this->objectpath))
        {
            core::glib::variant_cast(change, &device->hwAddress);
        }
    }

    void WiredDevice::on_property_active_connection(
        const Glib::VariantBase& change)
    {
        // Per NetworkManager DBus documentation, 'ActiveConnection' is a
        // property on ...Device, not ...Device.Wired, but in practice
        // that does not seem to be the case. So if we receive changes here,
        // pass them on to the underlying Device.
        if (auto device = this->container->get<Device>(this->objectpath))
        {
            device->on_property_active_connection(change);
        }
    }
}  // namespace platform::netconfig::dbus
