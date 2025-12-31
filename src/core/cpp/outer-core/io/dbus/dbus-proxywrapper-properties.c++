/// -*- c++ -*-
//==============================================================================
/// @file dbus-proxywrapper-properties.c++
/// @brief Abstract base for DBus Interface handlers
/// @author Tor Slettnes
//==============================================================================

#include "dbus-proxywrapper-properties.h++"
#include "glib-errors.h++"

namespace core::dbus
{
    constexpr auto properties_interface = "org.freedesktop.DBus.Properties";

    PropertiesProxy::PropertiesProxy(
        ProxyContainer* container,
        const dbus::ConnectionPtr& connection,
        const dbus::ServiceName& servicename,
        const dbus::ObjectPath& objectpath,
        const dbus::InterfaceName& interfacename)
        : ProxyWrapper(container,
                       connection,
                       servicename,
                       objectpath,
                       properties_interface),
          real_interface(interfacename)
    {
    }

    void PropertiesProxy::set_property(const std::string& propertyname,
                                       const Glib::VariantBase& value)
    {
        auto inputs = Glib::VariantContainerBase::create_tuple({
            Glib::Variant<Glib::ustring>::create(this->real_interface),
            Glib::Variant<Glib::ustring>::create(propertyname),
            Glib::Variant<Glib::VariantBase>::create(value),
        });

        logf_trace(
            "Invoking property proxy call, service=%r, path=%r, interface=%r:"
            " Set(%s)",
            this->servicename,
            this->objectpath,
            this->interfacename,
            inputs);

        try
        {
            this->proxy->call_sync("Set", inputs);
        }
        catch (...)
        {
            core::glib::log_exception(std::current_exception());
        }
    }

}  // namespace core::dbus
