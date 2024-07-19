/// -*- c++ -*-
//==============================================================================
/// @file dbus-proxywrapper.c++
/// @brief Abstract base for DBus Interface handlers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dbus-proxywrapper.h++"
#include "dbus-proxycontainer.h++"
#include "glib-variant.h++"
#include "glib-errors.h++"

namespace core::dbus
{
    //==========================================================================
    // ProxyContainer

    ProxyWrapper::ProxyWrapper(
        ProxyContainer* container,
        const ConnectionRef& connection,
        const ServiceName& servicename,
        const ObjectPath& objectpath,
        const InterfaceName& interfacename,
        SignalHandlerMap signal_handlers,
        PropertyHandlerMap property_handlers)
        : container(container),
          connection(connection),
          servicename(servicename),
          objectpath(objectpath),
          interfacename(interfacename),
          signal_handlers(signal_handlers),
          property_handlers(property_handlers),
          cancellable(Gio::Cancellable::create()),
          ready(false)
    {
        using namespace std::placeholders;

        Gio::DBus::Proxy::create(
            connection,
            servicename,
            objectpath,
            interfacename,
            std::bind(&ProxyWrapper::on_ready, this, _1, this->identifier()),
            this->cancellable);

        logf_trace("Created DBUS wrapper, name=%r, interfacename=%r, path=%r",
                   servicename,
                   interfacename,
                   objectpath);
    }

    std::string ProxyWrapper::shortpath() const
    {
        std::vector<std::string> parts = str::split(this->objectpath, "/");
        if (parts.size() >= 2)
        {
            return str::format(".../%s/%s",
                               parts.at(parts.size() - 2),
                               parts.at(parts.size() - 1));
        }
        else
        {
            return this->objectpath;
        }
    }

    std::string ProxyWrapper::shortname() const
    {
        return str::stem(this->interfacename, ".");
    }

    std::string ProxyWrapper::identifier() const
    {
        return str::format("%s.%s(%r)",
                           str::stem(this->servicename, "."),
                           str::stem(this->interfacename, "."),
                           this->shortpath());
    }

    void ProxyWrapper::on_ready(const Glib::RefPtr<Gio::AsyncResult>& result,
                                const std::string& identifier)
    {
        if (result)
        {
            try
            {
                logf_trace("%s.create_finish()", identifier);
                this->proxy = Gio::DBus::Proxy::create_finish(result);

                logf_trace("%s.initialize()", identifier);
                this->initialize();
            }
            catch (...)
            {
                core::glib::log_exception(std::current_exception(), identifier);
            }
        }
    }

    void ProxyWrapper::initialize()
    {
        this->initialize_properties();
        this->set_ready();
    }

    void ProxyWrapper::initialize_properties()
    {
        logf_trace("Adding %s PropertiesChanged handler", this->identifier());
        this->signal_handlers.insert_or_assign(
            "PropertiesChanged",
            [=](const Glib::VariantContainerBase& parameters) {
                this->on_properties_change(
                    core::glib::variant_cast<Gio::DBus::Proxy::MapChangedProperties>(
                        parameters, 0));
            });

        Gio::DBus::Proxy::MapChangedProperties props;
        for (const auto& [name, handler] : this->property_handlers)
        {
            logf_trace("Getting %s property %r", this->identifier(), name);
            Glib::VariantBase prop;
            this->proxy->get_cached_property(prop, name);
            props.emplace(name, prop);
        }

        this->update_properties(props);
    }

    void ProxyWrapper::call(const std::string& methodname,
                            const Glib::VariantContainerBase& parameters,
                            const Gio::SlotAsyncReady& slot) const
    {
        logf_trace("%s.%s(%s)", this->identifier(), methodname, parameters);
        // auto callback = sigc::bind(
        //     sigc::mem_fun(this, &ProxyWrapper::callback_handler),
        //     methodname,
        //     parameters,
        //     slot);
        this->proxy->call(methodname, slot, this->cancellable, parameters);
        // this->proxy->call(methodname, slot, this->cancellable, parameters);
    }

    void ProxyWrapper::callback_handler(
        ResultRef result,
        const std::string& methodname,
        const Glib::VariantContainerBase& parameters,
        const Gio::SlotAsyncReady& slot) const
    {
        try
        {
            slot(result);
        }
        catch (...)
        {
            core::glib::log_exception(
                std::current_exception(),
                str::format("%s.%s(%s)", this->identifier(), methodname, parameters));
        }
    }

    Glib::VariantContainerBase ProxyWrapper::call_finish(ResultRef result) const
    {
        // try
        // {
        return this->proxy->call_finish(result);
        // }
        // catch (const Glib::Error& e)
        // {
        //     throw gwrap::GlibError(e);
        // }
    }

    Glib::VariantContainerBase ProxyWrapper::call_sync(
        const std::string& methodname,
        const Glib::VariantContainerBase& parameters) const
    {
        std::string preamble = str::format("%s.%s(%s)",
                                           this->identifier(),
                                           methodname,
                                           parameters);
        logf_trace(preamble);
        try
        {
            return this->proxy->call_sync(methodname, this->cancellable, parameters);
        }
        catch (const Glib::Error& e)
        {
            throw core::glib::Error(e, preamble);
        }
    }

    void ProxyWrapper::set_ready()
    {
        core::signal::MappingAction action =
            this->ready ? core::signal::MAP_UPDATE
                        : core::signal::MAP_ADDITION;

        this->ready = true;
        this->emit_change(action);
        this->propagate_update(action);
        logf_debug("%s ready", this->identifier());
        this->container->set_wrapper_ready(this->shared_from_this());
    }

    void ProxyWrapper::on_remove()
    {
        logf_debug("Removing %s", this->identifier());
        if (!this->ready)
        {
            this->cancellable->cancel();
        }
        this->emit_change(core::signal::MAP_REMOVAL);
        this->propagate_update(core::signal::MAP_REMOVAL);
    }

    uint ProxyWrapper::update_properties(
        const Gio::DBus::Proxy::MapChangedProperties& changes)
    {
        uint numchanges = 0;
        for (const auto [property, data] : changes)
        {
            auto handler = this->property_handlers.find(property);
            if (handler != this->property_handlers.end())
            {
                logf_trace("Received %s property, invoking handler: %s = %s",
                           this->identifier(),
                           property,
                           data);
                handler->second(data);
                numchanges++;
            }
            else
            {
                logf_trace("Received %s property, no handler: %s = %s",
                           this->identifier(),
                           property,
                           data);
            }
        }
        logf_trace("%s updated %d properties", this->identifier(), numchanges);
        return numchanges;
    }

    void ProxyWrapper::on_properties_change(
        const Gio::DBus::Proxy::MapChangedProperties& changes,
        const std::vector<Glib::ustring>& invalidated)
    {
        uint numchanges = this->update_properties(changes);
        if (numchanges > 0 && this->ready)
        {
            this->emit_change(core::signal::MAP_UPDATE);
        }
    }

    void ProxyWrapper::on_signal(
        const std::string& sender_name,
        const std::string& signal_name,
        const Glib::VariantContainerBase& parameters)
    {
        auto handler = this->signal_handlers.find(signal_name);
        if (handler != this->signal_handlers.end())
        {
            logf_trace("Invoking %s signal %r handler, args=%s",
                       this->identifier(),
                       signal_name,
                       parameters);
            handler->second(parameters);
        }
    }

    ObjectPath ProxyWrapper::get_cached_path(const std::string& name) const
    {
        return this->get_cached_property<ObjectPath>(name);
    }

    bool ProxyWrapper::valid_path(const ObjectPath& path)
    {
        return path.size() > 1;
    }

    void ProxyWrapper::subscribe_updates(ProxyWrapper* requestor,
                                         ProxyWrapper::UpdateMethod updatemethod)
    {
        bool subscribe = false;
        if (this->ready)
        {
            logf_trace("Direct %s update from %s",
                       requestor->identifier(),
                       this->identifier());
            subscribe = updatemethod(this, core::signal::MAP_ADDITION);
        }
        else
        {
            logf_trace("Queueing %s update from %s",
                       requestor->identifier(),
                       this->identifier());
            subscribe = true;
        }

        if (subscribe)
        {
            std::weak_ptr<ProxyWrapper> weakref(requestor->shared_from_this());
            this->subscribers[requestor->objectpath] = {weakref, updatemethod};
        }
    }

    void ProxyWrapper::propagate_update(core::signal::MappingAction action)
    {
        if (this->subscribers.size())
        {
            logf_trace("Propagating %d %s updates from %s",
                       this->subscribers.size(),
                       action,
                       this->identifier());

            bool keep = true;
            for (auto it = this->subscribers.begin();
                 it != this->subscribers.end();
                 it = keep ? ++it : this->subscribers.erase(it))
            {
                const auto& [path, subscription] = *it;
                const auto& [weakref, updatemethod] = subscription;

                if (WrapperRef dep = weakref.lock())
                {
                    logf_trace("Deferred %s update from %s",
                               dep->identifier(),
                               this->identifier());
                    keep = updatemethod(this, action);
                }
                else
                {
                    logf_notice("Removing stale %s subscriber", this->identifier());
                    keep = false;
                }
            }
        }
    }
}  // namespace core::dbus
