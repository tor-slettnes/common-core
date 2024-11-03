/// -*- c++ -*-
//==============================================================================
/// @file dds-logsink.h++
/// @brief A logger backed using DDS
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "event-types.hpp"  // generated from `event-types.idl`
#include "dds-publisher.h++"
#include "logging/sinks/logsink.h++"
#include "logging/sinks/messageformatter.h++"
#include "types/create-shared.h++"

namespace core::dds
{
    class DDSLogger : public logging::LogSink,
                      public Publisher,
                      public core::types::enable_create_shared<DDSLogger>
    {
        using This = DDSLogger;
        using Super = logging::LogSink;

    protected:
        DDSLogger(const std::string &sink_id,
                  status::Level threshold,
                  const std::string &channel_name,
                  int domain_id);

    protected:
        void open() override;
        void close() override;
        void capture_event(const status::Event::ptr &event) override;

    private:
        DataWriterPtr<CC::Status::Event> log_writer;
    };

}  // namespace core::dds
