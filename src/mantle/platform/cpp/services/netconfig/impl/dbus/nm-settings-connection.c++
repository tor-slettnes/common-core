/// -*- c++ -*-
//==============================================================================
/// @file nm-settings-connection.c++
/// @brief DBus Proxy for Network Manager Settings.Connection objects
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "nm-settings-connection.h++"
#include "nm-device.h++"
#include "netconfig.h++"
#include "types/value.h++"
#include "status/exceptions.h++"

namespace platform::netconfig::dbus
{
    Connection::Connection(
        core::dbus::ProxyContainer* container,
        const core::dbus::ConnectionRef& connection,
        const core::dbus::ServiceName& servicename,
        const core::dbus::ObjectPath& objectpath)
        : MappedDataWrapper<ConnectionData>(
              container,
              connection,
              servicename,
              objectpath,
              NM_DBUS_INTERFACE_SETTINGS_CONNECTION,
              {
                  {"Updated", SLOT(Connection::on_signal_updated)},
              },
              {})
    {
    }

    std::shared_ptr<Connection> Connection::get_by_key(
        const Key& key,
        bool required)
    {
        if (auto ref = lookup<Connection>(key, false))
        {
            logf_trace("Found connection by UUID: id=%r, uuid=%s", ref->id, ref->uuid);
            return ref;
        }
        else
        {
            return Connection::get_by_id(key, required);
        }
    }

    std::shared_ptr<Connection> Connection::get_by_id(
        const std::string& id,
        bool required)
    {
        for (const auto& [path, ref] : dbus::container.instances<Connection>())
        {
            if (ref->id == id)
            {
                logf_trace("Found connection by ID: id=%r, uuid=%s",
                           ref->id,
                           ref->uuid);
                return ref;
            }
        }

        if (required)
        {
            throwf(core::exception::NotFound,
                   ("No such connection: %r", id),
                   id);
        }
        return {};
    }

    void Connection::initialize()
    {
        this->get_settings();
        // this->set_ready();
    }

    void Connection::remove()
    {
        logf_info("Removing connection, id=%r, uuid=%s, path=%s",
                  this->id,
                  this->uuid,
                  this->objectpath);
        this->call_sync("Delete");
    }

    void Connection::get_settings()
    {
        // this->call("GetSettings", {}, SLOT(Connection::on_ready_settings));
        // this->call("GetSecrets", {}, SLOT(Connection::on_ready_secrets));

        if (Glib::VariantContainerBase response = this->call_sync("GetSettings"))
        {
            if (response.get_n_children() > 0)
            {
                core::glib::SettingsContainer container;
                response.get_child(container, 0);
                connection::extract_settings(container, this);
            }

            logf_trace("Connection settings %s: %s",
                       this->identifier(),
                       response);

            this->set_ready();
        }
    }

    void Connection::set_ready()
    {
        if (!this->ready)
        {
            this->remove_duplicates();
        }

        Super::set_ready();
    }

    void Connection::on_signal_updated(const Glib::VariantContainerBase& parameters)
    {
        if (this->ready)
        {
            this->get_settings();
        }
    }

    void Connection::emit_change(core::signal::MappingAction action)
    {
        if (this->key().size())
        {
            logf_trace("signal_connection(%s, %r, %s)",
                       action,
                       this->key(),
                       *this);
            auto ref = std::static_pointer_cast<Connection>(shared_from_this());
            netconfig::signal_connection.emit(action, this->key(), ref);
        }
    }

    const core::types::ByteVector Connection::ssid() const
    {
        if (auto* wireless = std::get_if<WirelessConnectionData>(&this->specific_data))
        {
            return wireless->ssid;
        }
        else
        {
            return {};
        }
    }

    core::dbus::ObjectPath Connection::find_suitable_device()
    {
        core::dbus::WrapperRef ref;
        if (this->interface.size() > 0)
        {
            ref = lookup<Device>(this->interface, true);
        }
        else if (auto* wired = this->wired_data())
        {
            ref = WiredDevice::first();
        }
        else if (auto* wifi = this->wifi_data())
        {
            if (auto ap = AccessPoint::get_by_ssid(wifi->ssid, false))
            {
                ref = WirelessDevice::get_by_ap(ap, true);
            }
            else
            {
                ref = WirelessDevice::first();
            }
        }
        else
        {
            throwf(core::exception::InvalidArgument,
                   ("Connection %r has no interface name nor connection type ", id),
                   id);
        }
        return ref->objectpath;
    }

    void Connection::replace(const ConnectionData& data)
    {
        auto inputs = Glib::VariantContainerBase::create_tuple({
            connection::build_settings_container(data),
        });
        this->call_sync("Update", inputs);
    }

    void Connection::remove_duplicates()
    {
        for (const auto& [path, candidate] : this->container->instances<Connection>())
        {
            if ((candidate.get() != this) && (candidate->id == this->id))
            {
                candidate->remove();
            }
        }
    }
}  // namespace platform::netconfig::dbus
