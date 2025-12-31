/// -*- c++ -*-
//==============================================================================
/// @file dbus-proxycontainer.c++
/// @brief Abstract base for DBus Interface handlers
/// @author Tor Slettnes
//==============================================================================

#include "dbus-proxycontainer.h++"
#include "dbus-proxywrapper.h++"
#include "glib-errors.h++"
#include "string/misc.h++"

#include <glibmm/stringutils.h>

namespace core::dbus
{
    ProxyContainer::ProxyContainer(Gio::DBus::BusType bus,
                                   const ServiceName& servicename)
        : bus(bus),
          servicename(servicename)
    {
    }

    void ProxyContainer::connect()
    {
        if (!this->connection)
        {
            logf_debug("Establishing DBus connection to %r", this->servicename);
            this->connection = Gio::DBus::Connection::get_sync(this->bus);
            this->connection->signal_subscribe(
                sigc::mem_fun(*this, &ProxyContainer::on_signal),
                this->servicename);
        }
    }

    ConnectionPtr ProxyContainer::get_connection() const
    {
        return this->connection;
    }

    const ProxyContainer::WrapperMap ProxyContainer::map() const
    {
        return this->wrappers;
    }

    std::vector<ProxyWrapper::ptr> ProxyContainer::list() const
    {
        std::vector<ProxyWrapper::ptr> refs;
        for (auto pathitem : this->wrappers)
        {
            for (auto interfaceitem : pathitem.second)
            {
                refs.push_back(interfaceitem.second);
            }
        }
        return refs;
    }

    std::vector<ProxyWrapper::ptr> ProxyContainer::list(
        const InterfaceName& interfacename) const
    {
        std::vector<ProxyWrapper::ptr> refs;
        for (const ProxyWrapper::ptr& ref : this->list())
        {
            if (ref->interfacename == interfacename)
            {
                refs.push_back(ref);
            }
        }
        return refs;
    }

    std::vector<ProxyWrapper::ptr> ProxyContainer::list(
        const ObjectPath& prefix,
        const InterfaceName& interfacename) const
    {
        std::vector<ProxyWrapper::ptr> refs;
        for (auto pathitem : this->wrappers)
        {
            if (core::str::startswith(pathitem.first, prefix))
            {
                for (auto interfaceitem : pathitem.second)
                {
                    if (interfacename.empty() || (interfaceitem.first == interfacename))
                    {
                        refs.push_back(interfaceitem.second);
                    }
                }
            }
        }
        return refs;
    }

    ProxyWrapper::ptr ProxyContainer::add(ProxyWrapper::ptr wrapper)
    {
        const auto& [it, inserted] = this->wrappers[wrapper->objectpath]
                                         .emplace(wrapper->interfacename, wrapper);
        this->pending_init.insert(wrapper);
        return it->second;
    }

    ProxyContainer::WrapperMap::iterator ProxyContainer::remove(
        const ObjectPath& objectpath)
    {
        auto path_it = this->wrappers.find(objectpath);
        if (path_it != this->wrappers.end())
        {
            for (auto& [name, ref] : path_it->second)
            {
                this->pending_init.erase(ref);
                ref->on_remove();
            }
            return this->wrappers.erase(path_it);
        }
        return {};
    }

    // ProxyContainer::WrapperMap::iterator ProxyContainer::remove(WrapperMap::iterator it)
    // {
    //     for (const auto& [name, ref] : it->second)
    //     {
    //         ref->on_remove();
    //     }
    //     return this->wrappers.erase(it);
    // }

    ProxyWrapper::ptr ProxyContainer::get(
        const ObjectPath& objectpath,
        const InterfaceName& interfacename) const
    {
        ProxyWrapper::ptr ref;
        auto it1 = this->wrappers.find(objectpath);
        if (it1 != this->wrappers.end())
        {
            auto it2 = it1->second.find(interfacename);
            if (it2 != it1->second.end())
            {
                ref = it2->second;
            }
        }
        return ref;
    }

    void ProxyContainer::set_wrapper_ready(ProxyWrapper::ptr wrapper)
    {
        this->pending_init.erase(wrapper);
        if (this->pending_init.empty() && !this->ready)
        {
            this->set_ready();
        }
    }

    void ProxyContainer::set_ready()
    {
        this->ready = true;
        this->signal_ready.emit();
        logf_debug("ProxyContainer(%r) is ready", this->servicename);
    }

    bool ProxyContainer::is_ready()
    {
        return this->ready;
    }

    void ProxyContainer::on_signal(
        const Glib::RefPtr<Gio::DBus::Connection>& connection,
        const Glib::ustring& sender_name,
        const Glib::ustring& object_path,
        const Glib::ustring& interface_name,
        const Glib::ustring& signal_name,
        const Glib::VariantContainerBase& parameters)
    {
        if (object_path.size())
        {
            ObjectPath path(object_path);

            if (auto ref = this->get(path, interface_name))
            {
                std::string text = str::format(
                    "DBus signal %s, path=%r, interfacename=%r, sender=%r: %s",
                    signal_name,
                    path,
                    interface_name,
                    sender_name,
                    parameters);

                try
                {
                    logf_trace(text);
                    ref->on_signal(sender_name, signal_name, parameters);
                }
                catch (...)
                {
                    core::glib::log_exception(std::current_exception(), text);
                }
            }
        }
    }
}  // namespace core::dbus
