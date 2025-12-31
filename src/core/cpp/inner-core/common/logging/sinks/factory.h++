/// -*- c++ -*-
//==============================================================================
/// @file factory.h++
/// @brief Registry to catalogue and create log sinks
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "sink.h++"
#include "status/level.h++"

namespace core::logging
{
    enum class DefaultOption
    {
        DISABLED,
        ENABLED,
        IF_INTERACTIVE,
        UNLESS_INTERACTIVE
    };

    //--------------------------------------------------------------------------
    /// SinkFactory

    class SinkFactory
    {
    public:
        using CreatorFunction = std::function<Sink::ptr(const SinkID &)>;

    public:
        SinkFactory(const SinkType &sink_type,
                    const std::string &description,
                    const CreatorFunction &creator,
                    DefaultOption default_option = DefaultOption::DISABLED);

        virtual ~SinkFactory();

        Sink::ptr create_sink(const SinkID &sink_id);
        SinkType sink_type() const;
        std::string description() const;
        bool default_enabled() const;
        status::Level default_threshold(const types::KeyValueMap &settings) const;

    private:
        CreatorFunction creator() const;

    private:
        SinkType sink_type_;
        std::string description_;
        CreatorFunction creator_;
        DefaultOption default_option_;
    };

    inline types::ValueMap<SinkType, SinkFactory *> sink_factories;

    //--------------------------------------------------------------------------
    /// SinkCustomization

    class SinkCustomization
    {
    public:
        SinkCustomization(const SinkID &sink_id,
                          SinkFactory *factory,
                          const types::KeyValueMap &settings = {});

        void set_threshold(status::Level threshold);
        Sink::ptr activate();

    private:
        bool is_enabled() const;
        bool default_enabled() const;

    public:
        SinkID sink_id;
        SinkFactory *factory;
        types::KeyValueMap settings;
        std::optional<status::Level> explicit_threshold;
    };

    //--------------------------------------------------------------------------
    /// SinkRegistry

    class SinkRegistry : public types::ValueMap<SinkID, std::shared_ptr<SinkCustomization>>
    {
    public:
        void populate();
        types::ValueMap<SinkID, Sink::ptr> activate_sinks() const;
    };

    extern SinkRegistry sink_registry;

}  // namespace core::logging
