/// -*- c++ -*-
//==============================================================================
/// @file factory.h++
/// @brief Registry to catalogue and create log sinks
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sink.h++"

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
    /// \class SinkFactory

    class SinkFactory
    {
    public:
        using CreatorFunction = std::function<Sink::ptr(const SinkID &)>;

    public:
        SinkFactory(const std::string &sink_type,
                    const std::string &description,
                    const CreatorFunction &creator,
                    DefaultOption default_option = DefaultOption::DISABLED);

        virtual ~SinkFactory();

        Sink::ptr create_sink(const SinkID &sink_id,
                              const types::KeyValueMap &settings);

        SinkType sink_type() const;
        std::string description() const;
        bool default_enabled(const types::KeyValueMap &settings) const;

    private:
        CreatorFunction creator() const;

    private:
        SinkType sink_type_;
        std::string description_;
        CreatorFunction creator_;
        DefaultOption default_option_;
    };

    inline types::ValueMap<SinkType, SinkFactory *> sink_registry;
}  // namespace core::logging
