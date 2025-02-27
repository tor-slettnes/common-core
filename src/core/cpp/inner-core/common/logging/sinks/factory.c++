/// -*- c++ -*-
//==============================================================================
/// @file factory.c++
/// @brief Registry to catalogue and create log sinks
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "factory.h++"
#include "messagesink.h++"
#include "settings/settings.h++"
#include "platform/runtime.h++"
#include "platform/init.h++"

namespace core::logging
{
    //--------------------------------------------------------------------------
    /// @class SinkFactory

    SinkFactory::SinkFactory(const SinkType &sink_type,
                             const std::string &description,
                             const CreatorFunction &creator,
                             DefaultOption default_option)
        : sink_type_(sink_type),
          description_(description),
          creator_(creator),
          default_option_(default_option)
    {
        sink_factories.insert_or_assign(sink_type, this);
    }

    SinkFactory::~SinkFactory()
    {
        sink_factories.erase(this->sink_type());
    }

    Sink::ptr SinkFactory::create_sink(const SinkID &sink_id)
    {
        Sink::ptr sink = this->creator()(sink_id);
        sink->set_sink_type(this->sink_type());
        return sink;
    }

    SinkType SinkFactory::sink_type() const
    {
        return this->sink_type_;
    }

    std::string SinkFactory::description() const
    {
        return this->description_;
    }

    bool SinkFactory::default_enabled() const
    {
        switch (this->default_option_)
        {
        case DefaultOption::DISABLED:
            return false;

        case DefaultOption::ENABLED:
            return true;

        case DefaultOption::IF_INTERACTIVE:
            return platform::runtime->isatty(platform::STDIN);

        case DefaultOption::UNLESS_INTERACTIVE:
            return !platform::runtime->isatty(platform::STDIN);

        default:
            return false;
        }
    }

    status::Level SinkFactory::default_threshold(
        const types::KeyValueMap &settings) const
    {
        return settings
            .try_get_as<status::Level>("threshold")
            .value_or(status::Level::TRACE);
    }

    SinkFactory::CreatorFunction SinkFactory::creator() const
    {
        return this->creator_;
    }

    //--------------------------------------------------------------------------
    /// SinkCustomization

    SinkCustomization::SinkCustomization(const SinkID &sink_id,
                                         SinkFactory *factory,
                                         const types::KeyValueMap &settings)
        : sink_id(sink_id),
          factory(factory),
          settings(settings)
    {
    }

    void SinkCustomization::set_threshold(status::Level threshold)
    {
        this->explicit_threshold = threshold;
    }

    Sink::ptr SinkCustomization::activate()
    {
        if (this->is_enabled())
        {
            Sink::ptr sink = this->factory->create_sink(this->sink_id);
            sink->load_settings(this->settings);
            if (this->explicit_threshold)
            {
                if (auto logsink = std::dynamic_pointer_cast<MessageSink>(sink))
                {
                    logsink->set_threshold(this->explicit_threshold.value());
                }
            }
            sink->open();
            return sink;
        }
        else
        {
            return {};
        }
    }

    bool SinkCustomization::is_enabled() const
    {
        if (this->explicit_threshold)
        {
            return (this->explicit_threshold.value() != status::Level::NONE);
        }
        else if (const core::types::Value &setting = this->settings.get("enabled"))
        {
            return setting.as_bool();
        }
        else
        {
            return this->factory->default_enabled();
        }
    }

    //--------------------------------------------------------------------------
    /// SinkRegistry

    void SinkRegistry::populate()
    {
        std::set<std::string> consumed_sink_types;
        types::Value sink_settings = core::settings->get("log sinks");

        // First we add options to log to sinks with IDs corresponding to keys
        // from the "log sinks" section in settings. The settings for each sink
        // may include a "type" value, in which case the corresponding sink
        // factory is used.  Otherwise, the type is assumed to be the same as
        // the sink ID (e.g. "file").

        if (auto sink_map = sink_settings.get_kvmap())
        {
            types::Value sink_defaults = sink_map->get("_default_");

            for (const auto &[sink_id, sink_specs_value] : *sink_map)
            {
                std::string sink_type = sink_specs_value.get("type", sink_id).as_string();
                if (logging::SinkFactory *factory =
                        logging::sink_factories.get(sink_type, nullptr))
                {
                    consumed_sink_types.insert(sink_type);
                    auto specs = sink_specs_value.as_kvmap();
                    specs.recursive_merge(sink_defaults.as_kvmap());

                    this->insert_or_assign(
                        sink_id,
                        std::make_shared<SinkCustomization>(sink_id, factory, specs));
                }
            }
        }

        // We now add sink factories that wasn't yet mentioned, with sink IDs
        // matching the sink type name (e.g. "stderr").
        for (const auto &[sink_type, factory] : logging::sink_factories)
        {
            if (consumed_sink_types.count(sink_type) == 0)
            {
                this->insert_or_assign(
                    sink_type,
                    std::make_shared<SinkCustomization>(sink_type, factory));
            }
        }
    }

    types::ValueMap<SinkID, Sink::ptr> SinkRegistry::activate_sinks() const
    {
        types::ValueMap<SinkID, Sink::ptr> active_map;

        for (const auto &[id, customization] : *this)
        {
            if (Sink::ptr sink = customization->activate())
            {
                active_map.insert_or_assign(id, sink);
            }
        }

        return active_map;
    }

    SinkRegistry sink_registry;

    platform::InitTask populate_task(
        "PopulateMessageSinkRegistry",
        std::bind(&SinkRegistry::populate, &sink_registry));
}  // namespace core::logging
