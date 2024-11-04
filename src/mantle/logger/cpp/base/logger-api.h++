// -*- c++ -*-
//==============================================================================
/// @file logger-api.h++
/// @brief Logging service - abstract base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-types.h++"
#include "status/event.h++"

#include <optional>

namespace logger
{
    //--------------------------------------------------------------------------
    // API

    class API
    {
    protected:
        API(const std::string &identity);

    public:
        const std::string &identity() const;

        virtual void initialize() {}
        virtual void deinitialize() {}

        virtual void log(
            const core::status::Event::ptr &event) = 0;

        virtual bool add_sink(
            const SinkSpec &spec) = 0;

        virtual bool remove_sink(
            const SinkID &id) = 0;

        virtual SinkSpec get_sink_spec(
            const SinkID &id) const = 0;

        virtual SinkSpecs list_sinks() const = 0;

        virtual FieldNames list_static_fields() const = 0;

        virtual std::shared_ptr<EventSource> listen(
            const ListenerSpec &spec) = 0;

    private:
        std::string identity_;
    };
}  // namespace logger
