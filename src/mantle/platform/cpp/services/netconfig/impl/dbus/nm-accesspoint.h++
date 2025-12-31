/// -*- c++ -*-
//==============================================================================
/// @file nm-accesspoint.h++
/// @brief Specialized Gio::DBus::Proxy classes for NetConfig
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "nm-wrappers.h++"

namespace netconfig::dbus
{
    //==========================================================================
    /// Network Manager WiFi access point

    class AccessPoint : public MappedDataWrapper<AccessPointData>
    {
        using Class = AccessPoint;
        using Super = MappedDataWrapper<AccessPointData>;

    public:
        AccessPoint(
            core::dbus::ProxyContainer* container,
            const core::dbus::ConnectionPtr& connection,
            const core::dbus::ServiceName& servicename,
            const core::dbus::ObjectPath& objectpath);

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

}  // namespace netconfig::dbus
