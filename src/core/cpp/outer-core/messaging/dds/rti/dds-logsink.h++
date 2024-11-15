/// -*- c++ -*-
//==============================================================================
/// @file dds-logsink.h++
/// @brief A logger backed using DDS
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "event-types.hpp"  // generated from `event-types.idl`
#include "dds-publisher.h++"
#include "platform/path.h++"
#include "logging/sinks/factory.h++"
#include "logging/sinks/logsink.h++"
#include "logging/sinks/messageformatter.h++"
#include "types/create-shared.h++"

namespace core::dds
{
    //--------------------------------------------------------------------------
    // DDSLogger

    class DDSLogger : public logging::LogSink,
                      public logging::MessageFormatter,
                      public Publisher,
                      public core::types::enable_create_shared<DDSLogger>
    {
        using This = DDSLogger;
        using Super = logging::LogSink;

    protected:
        DDSLogger(const std::string &sink_id,
                  const std::string &channel_name = core::platform::path->exec_name(),
                  int domain_id = 0);

    protected:
        void load_settings(const types::KeyValueMap &settings) override;
        void open() override;
        void close() override;
        void capture_event(const status::Event::ptr &event) override;

    private:
        DataWriterPtr<CC::Status::Event> log_writer;
    };

    //--------------------------------------------------------------------------
    // Add factory option to enable sink

    inline static logging::SinkFactory dds_factory(
        "dds",
        "Enable logging over DDS [Default: %default]",
        [](const logging::SinkID &sink_id) -> logging::Sink::ptr {
            return DDSLogger::create_shared(sink_id);
        });
}  // namespace core::dds
