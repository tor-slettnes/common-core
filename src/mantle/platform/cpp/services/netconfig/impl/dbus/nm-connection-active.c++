/// -*- c++ -*-
//==============================================================================
/// @file nm-connection-active.c++
/// @brief DBus Proxy for Network Manager Connection.Active objects
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "nm-connection-active.h++"
#include "nm-ipconfig.h++"
#include "netconfig.h++"
#include "status/exceptions.h++"

namespace netconfig::dbus
{
    ActiveConnection::ActiveConnection(
        core::dbus::ProxyContainer *container,
        const core::dbus::ConnectionPtr &connection,
        const core::dbus::ServiceName &servicename,
        const core::dbus::ObjectPath &objectpath)
        : MappedDataWrapper<ActiveConnectionData>(
              container,
              connection,
              servicename,
              objectpath,
              NM_DBUS_INTERFACE_ACTIVE_CONNECTION,
              {
                  {"StateChanged", SLOT(Class::on_signal_state_changed)},
    },
              {
                  {"Id", DATASLOT(&this->id)},
                  {"Uuid", DATASLOT(&this->uuid)},
                  {"Type", SLOT(Class::on_property_type)},
                  {"State", DATASLOT(&this->state)},
                  {"StateFlags", DATASLOT(&this->state_flags)},
                  {"Default", DATASLOT(&this->default4)},
                  {"Ip4Config", SLOT(Class::on_property_ip4config)},
                  {"Default6", DATASLOT(&this->default6)},
                  {"Ip6Config", SLOT(Class::on_property_ip6config)},
              })
    {
        logf_trace("Adding active connection: %s", this->identifier());
    }

    std::shared_ptr<ActiveConnection> ActiveConnection::get_by_key(
        const Key &key,
        bool required)
    {
        if (auto ref = lookup<ActiveConnection>(key, false))
        {
            logf_trace("Found active connection by UUID: id=%r, uuid=%s",
                       ref->id,
                       ref->uuid);
            return ref;
        }
        else
        {
            return ActiveConnection::get_by_id(key, required);
        }
    }

    std::shared_ptr<ActiveConnection> ActiveConnection::get_by_id(
        const std::string &id,
        bool required)
    {
        for (const auto &[path, ref] : dbus::container.instances<ActiveConnection>())
        {
            if (ref->id == id)
            {
                logf_trace("Found active connection by ID: id=%r, uuid=%s",
                           ref->id,
                           ref->uuid);
                return ref;
            }
        }

        if (required)
        {
            throwf_args(core::exception::NotFound,
                        ("No such connection: %r", id),
                        id);
        }
        return {};
    }

    void ActiveConnection::emit_change(core::signal::MappingAction action)
    {
        if (this->key().size())
        {
            logf_trace("signal_active_connection(%s, %r, %s)",
                       action,
                       this->key(),
                       *this);

            auto ref = std::static_pointer_cast<ActiveConnection>(shared_from_this());
            netconfig::signal_active_connection.emit(action, this->key(), ref);
        }
    }

    void ActiveConnection::on_signal_state_changed(
        const Glib::VariantContainerBase &parameters)
    {
        core::glib::variant_cast(parameters, 0, &this->state);
        core::glib::variant_cast(parameters, 1, &this->state_reason);
        this->emit_change(core::signal::MAP_UPDATE);
    }

    void ActiveConnection::on_property_type(const Glib::VariantBase &change)
    {
        auto typestring = core::glib::variant_cast<std::string>(change);
        this->type = connection_type_map.from_string(typestring, CONN_TYPE_UNKNOWN);
    }

    void ActiveConnection::on_property_ip4config(const Glib::VariantBase &change)
    {
        auto path = core::glib::variant_cast<core::dbus::ObjectPath>(change);
        if (this->valid_path(path))
        {
            auto ref = this->container->add<IP4Config>(path);
            ref->subscribe_updates(this, SLOT(Class::update_ip4config));
        }
    }

    void ActiveConnection::on_property_ip6config(const Glib::VariantBase &change)
    {
        auto path = core::glib::variant_cast<core::dbus::ObjectPath>(change);
        if (this->valid_path(path))
        {
            auto ref = this->container->add<IP6Config>(path);
            ref->subscribe_updates(this, SLOT(Class::update_ip6config));
        }
    }

    bool ActiveConnection::update_ip4config(
        const core::dbus::ProxyWrapper *source,
        core::signal::MappingAction action)
    {
        if (const auto *datasource = dynamic_cast<const IP4Config *>(source))
        {
            this->ip4config = *datasource;
            this->emit_change(core::signal::MAP_UPDATE);
        }
        return true; // Stay subscribed to updates
    }

    bool ActiveConnection::update_ip6config(
        const core::dbus::ProxyWrapper *source,
        core::signal::MappingAction action)
    {
        if (const auto *datasource = dynamic_cast<const IP6Config *>(source))
        {
            this->ip6config = *datasource;
            this->emit_change(core::signal::MAP_UPDATE);
        }
        return true; // Stay subscribed to updates
    }
} // namespace netconfig::dbus
