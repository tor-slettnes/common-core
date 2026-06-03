/// -*- c++ -*-
//==============================================================================
/// @file nm-accesspoint.h++
/// @brief Specialized Gio::DBus::Proxy classes for NetConfig
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "nm-wrappers.h++"

namespace cc::platform::netconfig::dbus
{
    //==========================================================================
    /// Network Manager WiFi access point

    class AccessPoint : public MappedDataWrapper<AccessPointData>
    {
        using Class = AccessPoint;
        using Super = MappedDataWrapper<AccessPointData>;

    public:
        AccessPoint(
            cc::dbus::ProxyContainer* container,
            const cc::dbus::ConnectionPtr& connection,
            const cc::dbus::ServiceName& servicename,
            const cc::dbus::ObjectPath& objectpath);

        static std::shared_ptr<AccessPoint> get_by_ssid(
            const core::types::Bytes& ssid,
            bool required);

        std::string identifier() const override;

    protected:
        void emit_change(core::signal::MappingAction action) override;

    private:
        void on_property_lastseen(
            const Glib::VariantBase& change);
    };

}  // namespace cc::platform::netconfig::dbus
