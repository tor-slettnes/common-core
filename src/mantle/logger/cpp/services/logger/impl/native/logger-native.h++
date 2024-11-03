// -*- c++ -*-
//==============================================================================
/// @file logger-native.h++
/// @brief Logging service - host native interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-base.h++"

namespace logger
{
    class NativeLogger : public BaseLogger
    {
        using This = NativeLogger;
        using Super = BaseLogger;

    protected:
        NativeLogger(const std::string &identity);

    public:
        void log(
            const core::status::Event::ptr &event) override;

        bool add_sink(
            const SinkSpec &spec) override;

        bool remove_sink(
            const SinkID &id) override;

        SinkSpec get_sink_spec(
            const SinkID &id) const override;

        SinkSpecs list_sinks() const override;

        FieldNames list_static_fields() const override;

    protected:
        core::logging::Sink::ptr new_sink(const SinkSpec &spec) const;
        SinkSpec sink_spec(const core::logging::Sink::ptr &sink) const;
    };
}  // namespace logger
