/// -*- c++ -*-
//==============================================================================
/// @file dbus-proxycontainer.h++
/// @brief Abstract base for DBus Interface handlers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "dbus-types.h++"
#include "dbus-proxywrapper.h++"
#include "platform/symbols.h++"

namespace core::dbus
{
    class ProxyContainer
    {
        using WrapperMap = std::map<ObjectPath, std::map<InterfaceName, ProxyWrapper::ptr>>;

    public:
        ProxyContainer(Gio::DBus::BusType bus, const ServiceName& servicename);

        void connect();
        ConnectionPtr get_connection() const;

        const WrapperMap map() const;

        std::vector<ProxyWrapper::ptr> list() const;
        std::vector<ProxyWrapper::ptr> list(const InterfaceName& interfacename) const;
        std::vector<ProxyWrapper::ptr> list(const ObjectPath& prefix,
                                            const InterfaceName& interfacename) const;

        ProxyWrapper::ptr add(ProxyWrapper::ptr wrapper);
        WrapperMap::iterator remove(const ObjectPath& objectpath);
        // WrapperMap::iterator remove(WrapperMap::iterator it);
        ProxyWrapper::ptr get(const ObjectPath& objectpath,
                              const InterfaceName& interfacename) const;

        virtual void set_wrapper_ready(ProxyWrapper::ptr wrapper);
        virtual void set_ready();
        bool is_ready();

    public:
        template <class WrapperType>
        inline std::shared_ptr<WrapperType> get(const ObjectPath& objectpath)
        {
            auto it = this->wrappers.find(objectpath);
            if (it != this->wrappers.end())
            {
                for (const auto& [ifname, candidate] : it->second)
                {
                    if (auto ref = std::dynamic_pointer_cast<WrapperType>(candidate))
                    {
                        return ref;
                    }
                }
            }

            return {};
        }

        template <class WrapperType>
        inline std::shared_ptr<WrapperType> get()
        {
            for (const auto& [objectpath, objects] : this->wrappers)
            {
                for (const auto& [ifname, candidate] : objects)
                {
                    if (auto ref = std::dynamic_pointer_cast<WrapperType>(candidate))
                    {
                        return ref;
                    }
                }
            }
            return {};
        }

        /// Add a proxy wrapper for a predefined path, e.g.,
        /// /org/freedesktop/NetworkManager or /org/freedesktop/NetworkManager/Settings.
        template <class WrapperType>
        inline std::shared_ptr<WrapperType> add()
        {
            auto ref = std::make_shared<WrapperType>(
                this, this->connection, this->servicename);
            this->add(ref);
            return ref;
        }

        /// Add a proxy wrapper for a dynamic path, e.g.,
        /// /org/freedesktop/NetworkManager/1 or /org/freedesktop/NetworkManager/Settings/1.
        template <class WrapperType, class... Args>
        inline std::shared_ptr<WrapperType> add(
            const ObjectPath& objectpath,
            const Args&... args)
        {
            auto ref = this->get<WrapperType>(objectpath);
            if (!ref)
            {
                ref = std::make_shared<WrapperType>(
                    this,
                    this->connection,
                    this->servicename,
                    objectpath,
                    args...);
                this->add(ref);
            }
            return ref;
        }

        template <class WrapperType>
        inline void synchronize(const ObjectPaths& paths)
        {
            /// Keep track of object paths we want to keep
            std::set<ObjectPath> active;
            logf_trace("Synchronizing %s: %s", TYPE_NAME_BASE(WrapperType), paths);

            /// Go through the supplied list, add missing objects
            for (const ObjectPath& path : paths)
            {
                if (this->wrappers.count(path) == 0)
                {
                    this->add<WrapperType>(path);
                }
                active.insert(path);
            }

            /// Go through our current objects, remove those no longer present.
            for (auto it = this->wrappers.begin(); it != this->wrappers.end();)
            {
                const auto& [path, interfaces] = *it;
                bool erase = interfaces.empty();
                if (!erase && (active.count(path) == 0))
                {
                    for (auto& [ifname, ref] : interfaces)
                    {
                        if (std::dynamic_pointer_cast<WrapperType>(ref))
                        {
                            erase = true;
                            break;
                        }
                    }
                }
                if (erase)
                {
                    it = this->remove(path);
                }
                else
                {
                    ++it;
                }
            }
        }

        template <class WrapperType>
        inline std::map<ObjectPath, std::shared_ptr<WrapperType>> instances()
        {
            std::map<ObjectPath, std::shared_ptr<WrapperType>> instances;
            for (const auto& [path, interfacemap] : this->wrappers)
            {
                for (const auto& [name, wrapper] : interfacemap)
                {
                    if (auto ptr = std::dynamic_pointer_cast<WrapperType>(wrapper))
                    {
                        instances.emplace(path, ptr);
                    }
                }
            }
            return instances;
        }

    private:
        void on_signal(
            const Glib::RefPtr<Gio::DBus::Connection>& connection,
            const Glib::ustring& sender_name,
            const Glib::ustring& object_path,
            const Glib::ustring& interface_name,
            const Glib::ustring& signal_name,
            const Glib::VariantContainerBase& parameters);

    private:
        Gio::DBus::BusType bus;
        ConnectionPtr connection;
        std::string servicename;

    public:
        WrapperMap wrappers;
        bool ready;
        std::unordered_set<ProxyWrapper::ptr> pending_init;
        sigc::signal<void> signal_ready;
    };
}  // namespace core::dbus
