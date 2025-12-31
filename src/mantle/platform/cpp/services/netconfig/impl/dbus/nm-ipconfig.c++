/// -*- c++ -*-
//==============================================================================
/// @file nm-ipconfig.c++
/// @brief DBus Proxy for Network Manager IP settings
/// @author Tor Slettnes
//==============================================================================

#include "nm-ipconfig.h++"
#include "nm-structs.h++"

namespace netconfig::dbus
{
    //==========================================================================
    /// Network Manager IP Configuration

    void IPConfig::on_property_addressdata(
        const Glib::VariantBase& change)
    {
        auto addressdata = core::glib::variant_cast<core::glib::VariantMaps>(change);
        extract_addressdata(addressdata, &this->address_data);
    }

    void IPConfig::emit_change(core::signal::MappingAction action)
    {
        this->propagate_update(action);
    }

    //==========================================================================
    /// Network Manager IPv4 Configuration

    IP4Config::IP4Config(
        core::dbus::ProxyContainer* container,
        const core::dbus::ConnectionPtr& connection,
        const core::dbus::ServiceName& servicename,
        const core::dbus::ObjectPath& objectpath)
        : IPConfig(
              container,
              connection,
              servicename,
              objectpath,
              NM_DBUS_INTERFACE_IP4_CONFIG,
              {},
              {
                  {"AddressData", SLOT(Class::on_property_addressdata)},
                  {"Gateway", DATASLOT(&this->gateway)},
                  {"NameserverData", SLOT(Class::on_property_dns)},
              })
    {
    }

    void IP4Config::on_property_dns(
        const Glib::VariantBase& change)
    {
        auto serverdata = core::glib::variant_cast<core::glib::VariantMaps>(change);
        this->dns.clear();
        for (const auto& server : serverdata)
        {
            core::glib::extract_value(server, "address", &this->dns.emplace_back());
        }
    }

    //==========================================================================
    /// Network Manager IPv6 Configuration

    IP6Config::IP6Config(
        core::dbus::ProxyContainer* container,
        const core::dbus::ConnectionPtr& connection,
        const core::dbus::ServiceName& servicename,
        const core::dbus::ObjectPath& objectpath)
        : IPConfig(
              container,
              connection,
              servicename,
              objectpath,
              NM_DBUS_INTERFACE_IP6_CONFIG,
              {},
              {
                  {"AddressData", SLOT(Class::on_property_addressdata)},
                  {"Gateway", DATASLOT(&this->gateway)},
                  {"Nameservers", SLOT(Class::on_property_dns)},
              })
    {
    }

    void IP6Config::on_property_dns(
        const Glib::VariantBase& change)
    {
        core::glib::variant_cast(change, &this->dns);
    }

}  // namespace netconfig::dbus
