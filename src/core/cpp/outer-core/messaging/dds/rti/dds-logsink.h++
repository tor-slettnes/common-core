/// -*- c++ -*-
//==============================================================================
/// @file dds-logsink.h++
/// @brief A logger backed using DDS
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "event-types.hpp"  // generated from `event-types.idl`
#include "dds-publisher.h++"
#include "platform/path.h++"
#include "logging/sinks/factory.h++"
#include "logging/sinks/messagesink.h++"
#include "types/create-shared.h++"

namespace core::dds
{
    //--------------------------------------------------------------------------
    // DDSLogger

    class DDSLogger : public logging::MessageSink,
                      public Publisher,
                      public core::types::enable_create_shared<DDSLogger>
    {
        using This = DDSLogger;
        using Super = logging::MessageSink;

    protected:
        DDSLogger(const std::string &sink_id,
                  const std::string &channel_name = core::platform::path->exec_name(),
                  int domain_id = 0);

    protected:
        void open() override;
        void close() override;
        bool handle_message(const logging::Message::ptr &message) override;

    private:
        DataWriterPtr<CC::Status::LogMessage> log_writer;
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
