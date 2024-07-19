// -*- c++ -*-
//==============================================================================
/// @file network-provider.h++
/// @brief Network service - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "network-provider.h++"

namespace platform::network
{
    //==========================================================================
    // Provider class

    ConnectionRef Provider::get_connection(const Key &key) const
    {
        try
        {
            return this->get_connections().at(key);
        }
        catch (std::out_of_range)
        {
            return {};
        }
    }

    ConnectionRef Provider::get_connection_by_ssid(const SSID &ssid) const
    {
        for (auto &[name, ref] : this->get_connections())
        {
            if (auto *wifi = ref->wifi_data())
            {
                if (wifi->ssid == ssid)
                {
                    return ref;
                }
            }
        }
        return {};
    }

    ActiveConnectionRef Provider::get_active_connection(const Key &key) const
    {
        try
        {
            return this->get_active_connections().at(key);
        }
        catch (std::out_of_range)
        {
            return {};
        }
    }

    ActiveConnectionRef Provider::get_active_connection(ConnectionType type) const
    {
        for (const auto &[key, ref] : this->get_active_connections())
        {
            if (ref->type == type)
            {
                return ref;
            }
        }
        return {};
    }

    AccessPointRef Provider::get_ap(const Key &key) const
    {
        try
        {
            return this->get_aps().at(key);
        }
        catch (std::out_of_range)
        {
            return {};
        }
    }

    SSIDMap Provider::get_aps_by_ssid() const
    {
        SSIDMap map;
        for (const auto &[key, ap] : this->get_aps())
        {
            if (!ap->ssid.empty())
            {
                auto it = map.find(ap->ssid);
                if ((it == map.end()) || (ap->strength > it->second->strength))
                {
                    map.insert_or_assign(ap->ssid, ap);
                }
            }
        }
        return map;
    }

    std::vector<AccessPointRef> Provider::get_aps_by_strongest_ssid() const
    {
        std::multimap<core::types::Byte,
                      AccessPointRef,
                      std::greater<core::types::Byte>> mmap;

        for (const auto &[ssid, ap] : this->get_aps_by_ssid())
        {
            mmap.emplace(ap->strength, ap);
        }

        std::vector<AccessPointRef> aps;
        aps.reserve(mmap.size());
        for (const auto &[strength, ap] : mmap)
        {
            aps.push_back(ap);
        }
        return aps;
    }

    AccessPointRef Provider::get_active_ap() const
    {
        for (const auto &[key, device] : this->get_devices())
        {
            if (auto ap = this->get_active_ap(device))
            {
                return ap;
            }
        }
        return {};
    }

    AccessPointRef Provider::get_active_ap(const DeviceRef &dev) const
    {
        if (auto *wifi = dev->wifi_data())
        {
            if (wifi->active_accesspoint.size())
            {
                if (auto ap = this->get_ap(wifi->active_accesspoint))
                {
                    return ap;
                }
            }
        }
        return {};
    }

    std::set<SSID> Provider::get_active_ssids() const
    {
        std::set<SSID> active;

        for (const auto &[key, device] : this->get_devices())
        {
            if (auto *wifi = device->wifi_data())
            {
                if (wifi->active_accesspoint.size())
                {
                    if (auto ap = this->get_ap(wifi->active_accesspoint))
                    {
                        active.insert(ap->ssid);
                    }
                }
            }
        }
        return active;
    }

    bool Provider::ap_supported(AccessPointRef ap) const
    {
        switch (ap->auth_type())
        {
        case AUTH_TYPE_UNKNOWN:
            return false;

        case AUTH_TYPE_EAP:
            // return true;
            return ap->wpa_flags != 0x00;

        default:
            return true;
        }
    }

    DeviceRef Provider::get_device(const Key &key) const
    {
        try
        {
            return this->get_devices().at(key);
        }
        catch (std::out_of_range)
        {
            return {};
        }
    }

    DeviceRef Provider::get_device(NMDeviceType type) const
    {
        for (const auto &[key, ref] : this->get_devices())
        {
            if ((ref->type == type) &&
                (ref->state != NM_DEVICE_STATE_UNKNOWN) &&
                (ref->state != NM_DEVICE_STATE_UNMANAGED))
            {
                return ref;
            }
        }
        return {};
    }

    WifiTuple Provider::get_active_wifi_data() const
    {
        for (const auto &[key, dev] : this->get_devices())
        {
            if (const WirelessDeviceData *wifi = dev->wifi_data())
            {
                if (wifi->active_accesspoint.size())
                {
                    AccessPointRef ap = this->get_ap(wifi->active_accesspoint);
                    ActiveConnectionRef ac = this->get_active_connection(dev->active_connection);
                    ConnectionRef cs = this->get_connection(dev->active_connection);
                    return {dev, ap, ac, cs};
                }
            }
        }
        return {this->get_device(NM_DEVICE_TYPE_WIFI), {}, {}, {}};
    }

    bool Provider::has_gateway() const
    {
        for (const auto &[key, ac] : this->get_active_connections())
        {
            if (ac->has_gateway())
            {
                return true;
            }
        }
        return false;
    }

    NMConnectivityState Provider::get_connectivity() const
    {
        if (GlobalDataRef data = this->get_global_data())
        {
            return data->connectivity;
        }
        else
        {
            return NM_CONNECTIVITY_UNKNOWN;
        }
    }

    //==========================================================================
    // Provider instance

    core::platform::ProviderProxy<Provider> network("network");

}  // namespace platform::network

