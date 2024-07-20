/// -*- c++ -*-
//==============================================================================
/// @file nm-manager.c++
/// @brief Specialized Gio::DBus::Proxy classes for NetConfig
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "nm-manager.h++"
#include "types/value.h++"
#include "network-provider.h++"
#include "status/exceptions.h++"

#define SETTINGS_FILE        "netconfig.json"
#define SETTING_WIFI_ALLOWED "wifi allowed"
#define DEFAULT_WIFI_ALLOWED true
#define SETTING_WIFI_BAND    "wifi band"
#define DEFAULT_WIFI_BAND    "any"

namespace platform::network::dbus
{
    Manager::Manager(
        core::dbus::ProxyContainer* container,
        const core::dbus::ConnectionRef& connection,
        const core::dbus::ServiceName& servicename,
        const core::dbus::ObjectPath& objectpath)
        : DataWrapper<GlobalData>(
              container,
              connection,
              servicename,
              objectpath,
              NM_DBUS_INTERFACE,
              {
                  // {"DeviceAdded", SLOT(Manager::on_signal_device_added)},
                  // {"DeviceRemoved", SLOT(Manager::on_signal_device_removed)},
                  {"StateChanged", SLOT(Manager::on_signal_state_changed)},
              },
              {
                  {"Devices", SLOT(Manager::on_property_devices)},
                  {"State", DATASLOT(&this->state)},
                  {"WirelessEnabled", SLOT(Manager::on_property_wireless_enabled)},
                  {"WirelessHardwareEnabled", DATASLOT(&this->wireless_hardware_enabled)},
                  {"ActiveConnections", SLOT(Manager::on_property_active_connections)},
              }),
          properties_proxy(
              std::make_shared<core::dbus::PropertiesProxy>(
                  container,
                  connection,
                  servicename,
                  objectpath,
                  NM_DBUS_INTERFACE)),
          settings(SETTINGS_FILE)
    {
        this->wireless_allowed =
            this->settings.get(SETTING_WIFI_ALLOWED, DEFAULT_WIFI_ALLOWED).as_bool();

        this->wireless_band_selection =
            core::str::convert_to<WirelessBandSelection>(
                this->settings.get(SETTING_WIFI_BAND, {}).as_string(),
                BAND_ANY);

        logf_debug("Read band selection: %r -> %r",
                   this->settings.get(SETTING_WIFI_BAND, {}).as_string(),
                   this->wireless_band_selection);
    }

    void Manager::initialize()
    {
        Super::initialize();
        auto paths = this->get_cached_property<core::dbus::ObjectPaths>("Devices");
        this->container->synchronize<Device>(paths);
    }

    void Manager::emit_change(core::signal::MappingAction action)
    {
        auto ref = std::static_pointer_cast<Manager>(this->shared_from_this());
        network::signal_globaldata.emit(ref);
    }

    void Manager::on_signal_state_changed(const Glib::VariantContainerBase& parameters)
    {
        auto state = core::glib::variant_cast<uint>(parameters, 0);
        logf_trace("NetworkManager state change: %0x", state);
    }

    void Manager::on_property_devices(const Glib::VariantBase& change)
    {
        auto devices = core::glib::variant_cast<core::dbus::ObjectPaths>(change);
        this->container->synchronize<Device>(devices);
    }

    void Manager::on_property_active_connections(const Glib::VariantBase& change)
    {
        auto active_connections = core::glib::variant_cast<core::dbus::ObjectPaths>(change);
        logf_trace("NetworkManager active connections: %s", active_connections);
        this->container->synchronize<ActiveConnection>(active_connections);
    }

    void Manager::on_property_wireless_enabled(const Glib::VariantBase& change)
    {
        this->wireless_enabled = core::glib::variant_cast<bool>(change);
        if (this->wireless_enabled && !this->wireless_allowed)
        {
            this->set_wireless_enabled(false);
        }
    }

    void Manager::set_wireless_enabled(bool enabled)
    {
        if (enabled && !this->wireless_allowed)
        {
            throw core::exception::FailedPrecondition("WiFi is not allowed");
        }

        logf_debug("Setting wifi state = %b", enabled);
        this->properties_proxy->set_property("WirelessEnabled",
                                             Glib::Variant<bool>::create(enabled));
        this->wireless_enabled = enabled;
        this->emit_change(core::signal::MAP_UPDATE);
    }

    void Manager::set_wireless_allowed(bool allowed)
    {
        logf_debug("Setting wifi allowed = %b", allowed);
        this->settings.insert_or_assign(SETTING_WIFI_ALLOWED, allowed);
        this->wireless_allowed = allowed;
        if (!allowed && this->wireless_enabled)
        {
            this->set_wireless_enabled(false);
        }
        else
        {
            this->emit_change(core::signal::MAP_UPDATE);
        }
    }

    void Manager::select_wireless_band(WirelessBandSelection band)
    {
        this->wireless_band_selection = band;
        this->settings[SETTING_WIFI_BAND] = core::str::convert_from(band);
        this->settings.save();
    }

    void Manager::request_scan() const
    {
        for (const auto& [path, ref] : this->container->instances<WirelessDevice>())
        {
            ref->request_scan();
        }
    }

    void Manager::add_and_activate_connection(ConnectionData data)
    {
        if (WiredConnectionData* wired_data = data.wired_data())
        {
            if (auto dev = WiredDevice::first())
            {
                this->add_and_activate_connection(data, dev->objectpath);
            }
        }
        else if (WirelessConnectionData* wifi_data = data.wifi_data())
        {
            if (!wifi_data->band)
            {
                wifi_data->band = this->wireless_band_selection;
            }

            if (auto dev = WirelessDevice::first())
            {
                this->add_and_activate_connection(data, dev->objectpath);
            }
        }
        else
        {
            logf_notice("No connection type specified: %s", data);
        }
    }

    void Manager::add_and_activate_connection(
        const ConnectionData& data,
        const Glib::DBusObjectPathString& device_path,
        const Glib::DBusObjectPathString& specific_path)
    {
        auto settings = connection::build_settings_container(data);

        if (auto dev = this->container->get<Device>(device_path))
        {
            logf_info("Connecting dev=%s, data=%s",
                      dev->interface,
                      data);
        }

        auto inputs = Glib::VariantContainerBase::create_tuple({
            settings,
            core::dbus::ObjectPathVariant::create(device_path),
            core::dbus::ObjectPathVariant::create(specific_path),
        });

        this->call_sync("AddAndActivateConnection", inputs);
    }

    void Manager::activate_connection(const Key& key)
    {
        auto conn = Connection::get_by_key(key, true);
        core::dbus::ObjectPath devpath = conn->find_suitable_device();
        core::dbus::ObjectPath specificpath("/");

        logf_info("Activating connection: %s", conn->identifier());

        auto inputs = Glib::VariantContainerBase::create_tuple({
            core::dbus::ObjectPathVariant::create(conn->objectpath),
            core::dbus::ObjectPathVariant::create(devpath),
            core::dbus::ObjectPathVariant::create(specificpath),
        });

        this->call_sync("ActivateConnection", inputs);
    }

    void Manager::deactivate_connection(const Key& key)
    {
        if (auto ac = ActiveConnection::get_by_key(key, false))
        {
            auto inputs = Glib::VariantContainerBase::create_tuple({
                core::dbus::ObjectPathVariant::create(ac->objectpath),
            });

            this->call_sync("DeactivateConnection", inputs);
        }
    }

    void Manager::connect_ap(const Key& bssid,
                             const ConnectionData& data)
    {
        auto ap = lookup<AccessPoint>(bssid, true);
        auto dev = WirelessDevice::get_by_ap(ap, true);
        this->connect_ap(dev, ap, data);
    }

    void Manager::connect_ap(const SSID& ssid,
                             const ConnectionData& data)
    {
        auto ap = AccessPoint::get_by_ssid(ssid, true);
        auto dev = WirelessDevice::get_by_ap(ap, true);
        this->connect_ap(dev, ap, data);
    }

    void Manager::connect_ap(std::shared_ptr<WirelessDevice> wifidev,
                             std::shared_ptr<AccessPoint> ap,
                             const ConnectionData& data)
    {
        if (auto dev = this->container->get<Device>(wifidev->objectpath))
        {
            this->add_and_activate_connection(data,
                                              dev->objectpath,
                                              ap ? ap->objectpath : "/");
        }
    }

}  // namespace platform::network::dbus
