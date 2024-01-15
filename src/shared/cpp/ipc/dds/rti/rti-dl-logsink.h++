/// -*- c++ -*-
//==============================================================================
/// @file rti-dl-logsink.h++
/// @brief A logger backed using RTI Distributed Logger infrastructure
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "dds-channel.h++"
#include "logging/sinks/messagesink.h++"
#include "types/valuemap.h++"
#include "types/create-shared.h++"

#include <rti_dl/rti_dl_cpp.h>
#include <ndds/ndds_cpp.h>
#include <dds/domain/DomainParticipant.hpp>

namespace cc::logging
{
    class RTILogger : public MessageSink,
                      public cc::types::enable_create_shared<RTILogger>
    {
        using This = RTILogger;
        using Super = MessageSink;

    protected:
        RTILogger(const std::string &identity,
                  int domain_id);

    public:
        void set_threshold(status::Level threshold) override;

    protected:
        void open() override;
        void close() override;
        void capture_message(const Message::Ref &msg) override;

    private:
        static const types::ValueMap<status::Level, DDS_Long> levelmap;

    private:
        RTI_DLOptions options_;
        RTI_DLDistLogger *dist_logger_;
    };

}  // namespace cc::logging
