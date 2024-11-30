// -*- c++ -*-
//==============================================================================
/// @file netconfig.c++
/// @brief NetConfig service - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "netconfig.h++"

namespace platform::netconfig
{
    //==========================================================================
    // Provider class

    ConnectionData::ptr ProviderInterface::get_connection(const Key &key) const
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

    ConnectionData::ptr ProviderInterface::get_connection_by_ssid(const SSID &ssid) const
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

    ActiveConnectionData::ptr ProviderInterface::get_active_connection(const Key &key) const
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

    ActiveConnectionData::ptr ProviderInterface::get_active_connection(ConnectionType type) const
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

    AccessPointData::ptr ProviderInterface::get_ap(const Key &key) const
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

    SSIDMap ProviderInterface::get_aps_by_ssid() const
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

    std::vector<AccessPointData::ptr> ProviderInterface::get_aps_by_strongest_ssid() const
    {
        std::multimap<core::types::Byte,
                      AccessPointData::ptr,
                      std::greater<core::types::Byte>>
            mmap;

        for (const auto &[ssid, ap] : this->get_aps_by_ssid())
        {
            mmap.emplace(ap->strength, ap);
        }

        std::vector<AccessPointData::ptr> aps;
        aps.reserve(mmap.size());
        for (const auto &[strength, ap] : mmap)
        {
            aps.push_back(ap);
        }
        return aps;
    }

    AccessPointData::ptr ProviderInterface::get_active_ap() const
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

    AccessPointData::ptr ProviderInterface::get_active_ap(const DeviceData::ptr &dev) const
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

    std::set<SSID> ProviderInterface::get_active_ssids() const
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

    bool ProviderInterface::ap_supported(AccessPointData::ptr ap) const
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

    DeviceData::ptr ProviderInterface::get_device(const Key &key) const
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

    DeviceData::ptr ProviderInterface::get_device(NMDeviceType type) const
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

    WifiTuple ProviderInterface::get_active_wifi_data() const
    {
        for (const auto &[key, dev] : this->get_devices())
        {
            if (const WirelessDeviceData *wifi = dev->wifi_data())
            {
                if (wifi->active_accesspoint.size())
                {
                    AccessPointData::ptr ap = this->get_ap(wifi->active_accesspoint);
                    ActiveConnectionData::ptr ac = this->get_active_connection(dev->active_connection);
                    ConnectionData::ptr cs = this->get_connection(dev->active_connection);
                    return {dev, ap, ac, cs};
                }
            }
        }
        return {this->get_device(NM_DEVICE_TYPE_WIFI), {}, {}, {}};
    }

    bool ProviderInterface::has_gateway() const
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

    NMConnectivityState ProviderInterface::get_connectivity() const
    {
        if (GlobalData::ptr data = this->get_global_data())
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

    core::platform::ProviderProxy<ProviderInterface> network("network");

}  // namespace platform::netconfig
