/// -*- c++ -*-
//==============================================================================
/// @file nm-accesspoint.c++
/// @brief DBus Proxy for Network Manager AccessPoint objects
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "nm-accesspoint.h++"
#include "nm-manager.h++"
#include "types/value.h++"
#include "netconfig.h++"
#include "status/exceptions.h++"

namespace platform::netconfig::dbus
{
    AccessPoint::AccessPoint(
        core::dbus::ProxyContainer* container,
        const core::dbus::ConnectionRef& connection,
        const core::dbus::ServiceName& servicename,
        const core::dbus::ObjectPath& objectpath)
        : MappedDataWrapper<AccessPointData>(
              container,
              connection,
              servicename,
              objectpath,
              NM_DBUS_INTERFACE_ACCESS_POINT,
              {},
              {
                  {"Ssid", DATASLOT(static_cast<core::types::Bytes*>(&this->ssid))},
                  {"Frequency", DATASLOT(&this->frequency)},
                  {"Flags", DATASLOT(&this->flags)},
                  {"WpaFlags", DATASLOT(&this->wpa_flags)},
                  {"RsnFlags", DATASLOT(&this->rsn_flags)},
                  {"HwAddress", DATASLOT(&this->hwAddress)},
                  {"Mode", DATASLOT(&this->mode)},
                  {"MaxBitrate", DATASLOT(&this->maxbitrate)},
                  {"Strength", DATASLOT(&this->strength)},
                  {"LastSeen", SLOT(Class::on_property_lastseen)},
              })
    {
        // this->path = objectpath;
    }

    std::shared_ptr<AccessPoint> AccessPoint::get_by_ssid(
        const core::types::Bytes& ssid,
        bool required)
    {
        std::shared_ptr<AccessPoint> best = {};

        for (const auto& [path, candidate] : dbus::container.instances<AccessPoint>())
        {
            if ((candidate->ssid == ssid) &&
                (!best || best->strength < candidate->strength))
            {
                logf_trace("Found AP for SSID %r, strength=%d, bssid=%s, path=%s",
                           candidate->ssid.to_string(),
                           candidate->strength,
                           candidate->hwAddress,
                           path);
                best = candidate;
            }
        }
        if (required && !best)
        {
            throwf(core::exception::NotFound,
                   ("No access point with such SSID found: %s",
                    core::types::ByteVector(ssid).to_string()),
                   ssid);
        }

        return best;
    }

    std::string AccessPoint::identifier() const
    {
        return core::str::format("%s.%s(%r, %r, %r)",
                                 core::str::stem(this->servicename, "."),
                                 core::str::stem(this->interfacename, "."),
                                 this->shortpath(),
                                 this->key(),
                                 this->ssid.to_string());
    }

    void AccessPoint::on_property_lastseen(
        const Glib::VariantBase& change)
    {
        int seconds = core::glib::variant_cast<int>(change);
        std::chrono::duration uptime = core::steady::Clock::now().time_since_epoch();
        this->lastSeen = core::dt::Clock::now() - uptime + std::chrono::seconds(seconds);
    }

    void AccessPoint::emit_change(core::signal::MappingAction action)
    {
        bool relevant = true;
        switch (action)
        {
        case core::signal::MAP_ADDITION:
        case core::signal::MAP_UPDATE:
            if (auto mgr = this->container->get<Manager>())
            {
                relevant = (mgr->wireless_band_selection == BAND_ANY) ||
                           (this->band() == mgr->wireless_band_selection);
            }
            break;

        case core::signal::MAP_REMOVAL:
            relevant = netconfig::signal_accesspoint.get_cached(this->key()).has_value();
            break;
        }

        if (relevant)
        {
            logf_trace("signal_accesspoint: %s", *this);
            auto ref = std::static_pointer_cast<AccessPoint>(shared_from_this());
            netconfig::signal_accesspoint.emit(action, this->key(), ref);
        }
    }

}  // namespace platform::netconfig::dbus
