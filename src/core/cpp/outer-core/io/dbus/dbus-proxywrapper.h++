/// -*- c++ -*-
//==============================================================================
/// @file dbus-proxywrapper.h++
/// @brief Abstract base for DBus Interface handlers
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "dbus-types.h++"
#include "glib-variant.h++"
#include "thread/signaltemplate.h++"

#include <unordered_map>
#include <map>
#include <set>
#include <functional>
#include <type_traits>
#include <iostream>

namespace core::dbus
{
    class ProxyWrapper : public std::enable_shared_from_this<ProxyWrapper>
    {
    public:
        using ptr = std::shared_ptr<ProxyWrapper>;

    private:
        friend class ProxyContainer;

        // Callback method to hande updates propagated from another object.
        using UpdateMethod = std::function<bool(const ProxyWrapper* source,
                                                core::signal::MappingAction change)>;
        using UpdateRequest = std::pair<std::weak_ptr<ProxyWrapper>, UpdateMethod>;

        // using SignalHandler = void (ProxyWrapper::*)(const Glib::VariantContainerBase& params);
        // using PropertyHandler = void (ProxyWrapper::*)(const Glib::VariantBase& change);

        using SignalHandler = std::function<void(const Glib::VariantContainerBase&)>;
        using PropertyHandler = std::function<void(const Glib::VariantBase&)>;

        using SignalHandlerMap = std::unordered_map<SignalName, SignalHandler>;
        using PropertyHandlerMap = std::unordered_map<PropertyName, PropertyHandler>;

        using ResultPtr = Glib::RefPtr<Gio::AsyncResult>;

    protected:
        ProxyWrapper(
            ProxyContainer* container,
            const ConnectionPtr& connection,
            const ServiceName& servicename,
            const ObjectPath& objectpath,
            const InterfaceName& interfacename,
            SignalHandlerMap signal_handlers = {},
            PropertyHandlerMap property_handlers = {});

    public:
        std::string shortpath() const;
        std::string shortname() const;
        virtual std::string identifier() const;

    private:
        void on_ready(const ResultPtr& result, const std::string& identifier);

    protected:
        virtual void initialize();
        virtual void initialize_properties();
        virtual void set_ready();
        virtual void on_remove();

        void call(
            const std::string& methodname,
            const Glib::VariantContainerBase& parameters = {},
            const Gio::SlotAsyncReady& slot = {}) const;

        void callback_handler(
            ResultPtr result,
            const std::string& methodname,
            const Glib::VariantContainerBase& parameters,
            const Gio::SlotAsyncReady& slot) const;

        Glib::VariantContainerBase call_finish(
            ResultPtr result) const;

        Glib::VariantContainerBase call_sync(
            const std::string& methodname,
            const Glib::VariantContainerBase& parameters = {}) const;

    public:
        void subscribe_updates(ProxyWrapper* requestor, UpdateMethod method);

    protected:
        virtual void propagate_update(core::signal::MappingAction change);

        virtual uint update_properties(
            const Gio::DBus::Proxy::MapChangedProperties& changes);

        void on_signal(
            const std::string& sender_name,
            const std::string& signal_name,
            const Glib::VariantContainerBase& parameters);

        void on_properties_change(
            const Gio::DBus::Proxy::MapChangedProperties& changes,
            const std::vector<Glib::ustring>& invalidated = {});

        virtual void emit_change(core::signal::MappingAction action) {}

        ObjectPath get_cached_path(const std::string& name) const;

        static bool valid_path(const ObjectPath& path);

    protected:
        template <class T>
        inline bool get_cached_property(const std::string& name, T* value) const
        {
            Glib::VariantBase prop;
            if (this->proxy)
            {
                this->proxy->get_cached_property(prop, name);
            }

            if (prop)
            {
                return core::glib::variant_cast<T>(prop, value);
            }
            else
            {
                logf_notice("ProxyWrapper %s [%s] (%s) could not get cached property %r",
                            this->objectpath,
                            this->interfacename,
                            this->proxy ? this->proxy->get_name() : "no DBus proxy",
                            name);
                return false;
            }
        }

        template <class T>
        inline T get_cached_property(const std::string& name) const
        {
            T value = {};
            get_cached_property<T>(name, &value);
            return value;
        }

    protected:
        ProxyContainer* container;

    public:
        ConnectionPtr connection;
        const std::string servicename;
        const ObjectPath objectpath;
        const InterfaceName interfacename;

    private:
        SignalHandlerMap signal_handlers;
        PropertyHandlerMap property_handlers;

    public:
        bool ready = false;
        Glib::RefPtr<Gio::Cancellable> cancellable;
        Glib::RefPtr<Gio::DBus::Proxy> proxy;
        std::map<ObjectPath, UpdateRequest> subscribers;
    };  // namespace dbus

}  // namespace core::dbus
