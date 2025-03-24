/// -*- c++ -*-
//==============================================================================
/// @file nm-ipconfig.h++
/// @brief Specialized Gio::DBus::Proxy classes for NetConfig
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "nm-wrappers.h++"

namespace netconfig::dbus
{
    //==========================================================================
    /// Network Manager IP Configuration (Base for IPv4 and IPv6)

    class IPConfig : public DataWrapper<IPConfigData>
    {
        using Class = IPConfig;
        using Super = DataWrapper<IPConfigData>;

    public:
        using DataWrapper<IPConfigData>::DataWrapper;

    protected:
        void emit_change(core::signal::MappingAction action) override;

        void on_property_addressdata(
            const Glib::VariantBase& change);
    };

    //==========================================================================
    /// Network Manager IP4 Configuration

    class IP4Config : public IPConfig
    {
        using Class = IP4Config;
        using Super = IPConfig;

    public:
        IP4Config(
            core::dbus::ProxyContainer* container,
            const core::dbus::ConnectionPtr& connection,
            const core::dbus::ServiceName& servicename,
            const core::dbus::ObjectPath& objectpath);

    protected:
        void on_property_dns(
            const Glib::VariantBase& change);
    };

    //==========================================================================
    /// Network Manager IP6 Configuration

    class IP6Config : public IPConfig
    {
        using Class = IP6Config;
        using Super = IPConfig;

    public:
        IP6Config(
            core::dbus::ProxyContainer* container,
            const core::dbus::ConnectionPtr& connection,
            const core::dbus::ServiceName& servicename,
            const core::dbus::ObjectPath& objectpath);

    protected:
        void on_property_dns(
            const Glib::VariantBase& change);
    };

}  // namespace netconfig::dbus
