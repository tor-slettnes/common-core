// -*- c++ -*-
//==============================================================================
/// @file netconfig-provider-dbus.c++
/// @brief NetConfig service - D-BUS implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "netconfig-dbus.h++"
#include "dbus-proxycontainer.h++"
#include "logging/logging.h++"

namespace platform::netconfig::dbus
{
    //==========================================================================
    /// NetConfig provider
    DBusProvider::DBusProvider()
        : Super("DBusProvider")
    {
    }

    bool DBusProvider::is_pertinent() const
    {
        try
        {
            dbus::container.connect();
            return bool(container.get_connection());
        }
        catch (...)
        {
            logf_notice("%s failed to establish DBus connection: %s",
                        this->name(),
                        std::current_exception());
            return false;
        }
    }

    void DBusProvider::initialize()
    {
        Super::initialize();
        logf_trace("Creating proxy container.");

        this->manager = dbus::container.add<Manager>();
        this->settings = dbus::container.add<Settings>();
    }

    //======================================================================
    // Settings methods

    std::string DBusProvider::get_hostname() const
    {
        return this->settings->hostname;
    }

    void DBusProvider::set_hostname(const std::string &hostname)
    {
        this->settings->set_hostname(hostname);
    }

    //==========================================================================
    // Connection methods

    ConnectionMap DBusProvider::get_connections() const
    {
        return datamap<Connection, ConnectionData>();
    }

    void DBusProvider::define_connection(const ConnectionData &data,
                                         bool activate)
    {
        if (activate)
        {
            if (auto conn = Connection::get_by_key(data.key(), false))
            {
                conn->replace(data);
                this->manager->activate_connection(data.key());
            }
            else
            {
                this->manager->add_and_activate_connection(data);
            }
        }
        else
        {
            this->settings->define_connection(data);
        }
    }

    bool DBusProvider::remove_connection(const Key &key)
    {
        return this->settings->remove_connection(key);
    }

    void DBusProvider::activate_connection(const Key &key)
    {
        return this->manager->activate_connection(key);
    }

    void DBusProvider::deactivate_connection(const Key &key)
    {
        return this->manager->deactivate_connection(key);
    }

    //==========================================================================
    // ActiveConnection Methods

    ActiveConnectionMap DBusProvider::get_active_connections() const
    {
        return datamap<ActiveConnection, ActiveConnectionData>();
    }

    //==========================================================================
    // AccessPoint Methods

    void DBusProvider::request_scan()
    {
        this->manager->request_scan();
    }

    AccessPointMap DBusProvider::get_aps() const
    {
        AccessPointMap apmap = datamap<AccessPoint, AccessPointData>();
        if (this->manager->wireless_band_selection != BAND_ANY)
        {
            auto it = apmap.begin();
            while (it != apmap.end())
            {
                if (it->second->band() != this->manager->wireless_band_selection)
                {
                    it = apmap.erase(it);
                }
                else
                {
                    it++;
                }
            }
        }
        return apmap;
    }

    void DBusProvider::connect_ap(const Key &bssid,
                                  const ConnectionData &connection)
    {
        return this->manager->connect_ap(bssid, connection);
    }

    void DBusProvider::connect_ap(const SSID &ssid,
                                  const ConnectionData &connection)
    {
        return this->manager->connect_ap(ssid, connection);
    }

    //==========================================================================
    // Device Methods

    DeviceMap DBusProvider::get_devices() const
    {
        return datamap<Device, DeviceData>();
    }

    //======================================================================
    // Manager methods

    GlobalData::ptr DBusProvider::get_global_data() const
    {
        return this->manager;
    }

    void DBusProvider::set_wireless_enabled(bool enabled)
    {
        this->manager->set_wireless_enabled(enabled);
    }

    void DBusProvider::set_wireless_allowed(bool allowed)
    {
        this->manager->set_wireless_allowed(allowed);
    }

    void DBusProvider::select_wireless_band(WirelessBandSelection band)
    {
        this->manager->select_wireless_band(band);
    }

}  // namespace platform::netconfig::dbus
