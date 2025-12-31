/// -*- c++ -*-
//==============================================================================
/// @file nm-settings.c++
/// @brief DBus Proxy for Network Manager Settings object
/// @author Tor Slettnes
//==============================================================================

#include "nm-settings.h++"
#include "nm-settings-connection.h++"
#include "sysconfig-host.h++"
#include "status/exceptions.h++"

namespace netconfig::dbus
{
    Settings::Settings(
        core::dbus::ProxyContainer *container,
        const core::dbus::ConnectionPtr &connection,
        const core::dbus::ServiceName &servicename,
        const core::dbus::ObjectPath &objectpath)
        : DataWrapper<SystemData>(
              container,
              connection,
              servicename,
              objectpath,
              NM_DBUS_INTERFACE_SETTINGS,
              {
                  {"NewConnection",     SLOT(Settings::on_signal_connection_added)  },
                  {"ConnectionRemoved", SLOT(Settings::on_signal_connection_removed)},
    },
              {
                  {"Hostname", DATASLOT(&this->hostname)},
              })
    {
    }

    void Settings::initialize_properties()
    {
        Super::initialize_properties();
        auto connections = this->get_cached_property<core::dbus::ObjectPaths>("Connections");
        this->container->synchronize<Connection>(connections);
    }

    void Settings::define_connection(const ConnectionData &data)
    {
        if (auto conn = Connection::get_by_id(data.id, false))
        {
            logf_info("Replacing existing connection data: %r", data);
            conn->replace(data);
        }
        else
        {
            logf_info("Adding new connection data: %r", data);
            this->add_connection(data);
        }
    }

    void Settings::add_connection(const ConnectionData &data)
    {
        auto inputs = Glib::VariantContainerBase::create_tuple({
            connection::build_settings_container(data),
        });

        this->call_sync("AddConnection", inputs);
    }

    bool Settings::remove_connection(const std::string &key, bool required)
    {
        bool found = false;
        if (auto ref = lookup<Connection>(key, false)) // UUID found
        {
            ref->remove();
            found = true;
        }
        else
        {
            for (const auto &[path, ref] : this->container->instances<Connection>())
            {
                if (ref->id == key)
                {
                    ref->remove();
                    found = true;
                }
            }
        }

        if (!found)
        {
            if (required)
            {
                throwf_args(core::exception::NotFound,
                            ("No such connection exists: %r", key),
                            key);
            }
            else
            {
                logf_info("Ignoring request to remove non-existing connection %r", key);
            }
        }
        return found;
    }

    void Settings::set_hostname(const std::string &hostname)
    {
        auto inputs = Glib::VariantContainerBase::create_tuple({
            Glib::Variant<Glib::ustring>::create(hostname),
        });
        this->call("SaveHostname", inputs);
    }

    void Settings::on_signal_connection_added(
        const Glib::VariantContainerBase &parameters)
    {
        auto path = core::glib::variant_cast<core::dbus::ObjectPath>(parameters, 0);
        logf_debug("Added connection %r", path);
        auto conn = this->container->add<Connection>(path);
    }

    void Settings::on_signal_connection_removed(
        const Glib::VariantContainerBase &parameters)
    {
        auto path = core::glib::variant_cast<core::dbus::ObjectPath>(parameters, 0);
        logf_debug("Removed connection %r", path);
        auto conn = this->container->remove(path);
    }

    void Settings::on_property_hostname(const Glib::VariantBase &change)
    {
        auto hostname = core::glib::variant_cast<std::string>(change);
        logf_debug("signal_hostname: %s", hostname);
        sysconfig::signal_hostinfo.emit(sysconfig::host->get_host_info());
    }

} // namespace netconfig::dbus
