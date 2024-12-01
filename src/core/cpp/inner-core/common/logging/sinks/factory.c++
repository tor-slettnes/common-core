/// -*- c++ -*-
//==============================================================================
/// @file factory.c++
/// @brief Registry to catalogue and create log sinks
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "factory.h++"
#include "platform/runtime.h++"

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
        sink_registry.insert_or_assign(sink_type, this);
    }

    SinkFactory::~SinkFactory()
    {
        sink_registry.erase(this->sink_type());
    }

    Sink::ptr SinkFactory::create_sink(const SinkID &sink_id,
                                       const types::KeyValueMap &settings)
    {
        Sink::ptr sink = this->creator()(sink_id);
        sink->set_sink_type(this->sink_type());
        sink->load_settings(settings);
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

    bool SinkFactory::default_enabled(const types::KeyValueMap &settings) const
    {
        if (const core::types::Value &setting = settings.get("enabled"))
        {
            return setting.as_bool();
        }
        else
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
    }

    SinkFactory::CreatorFunction SinkFactory::creator() const
    {
        return this->creator_;
    }

}  // namespace core::logging
