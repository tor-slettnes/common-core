/// -*- c++ -*-
//==============================================================================
/// @file dds-logsink.h++
/// @brief A logger backed using DDS
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "event-types.hpp"  // generated from `event-types.idl`
#include "dds-publisher.h++"
#include "logging/sinks/messagesink.h++"
#include "types/create-shared.h++"

namespace core::dds
{
    class DDSLogger : public logging::MessageSink,
                      public Publisher,
                      public core::types::enable_create_shared<DDSLogger>
    {
        using This = DDSLogger;
        using Super = logging::MessageSink;

    protected:
        DDSLogger(const std::string &channel_name, int domain_id);

    protected:
        void open() override;
        void close() override;
        void capture_message(const logging::Message::Ref &msg) override;

    private:
        DataWriterRef<CC::Status::LogMessage> log_writer;
    };

}  // namespace core::dds
