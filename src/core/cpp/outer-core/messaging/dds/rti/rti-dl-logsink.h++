/// -*- c++ -*-
//==============================================================================
/// @file rti-dl-logsink.h++
/// @brief A logger backed using RTI Distributed Logger infrastructure
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logging/sinks/logsink.h++"
#include "logging/sinks/messageformatter.h++"
#include "types/valuemap.h++"
#include "types/create-shared.h++"

#include <dds/domain/DomainParticipant.hpp>
#include <rti_dl/rti_dl_cpp.h>
#include <ndds/ndds_cpp.h>

namespace core::dds
{
    class RTIDistributedLogger : public core::logging::LogSink,
                                 public core::logging::MessageFormatter,
                                 public core::types::enable_create_shared<RTIDistributedLogger>
    {
        using This = RTIDistributedLogger;
        using Super = core::logging::LogSink;

    protected:
        RTIDistributedLogger(const std::string &sink_id,
                             const std::string &application_id,
                             int domain_id);

    protected:
        void load_settings(const types::KeyValueMap &settings) override;

    public:
        void set_threshold(core::status::Level threshold) override;

    protected:
        void open() override;
        void close() override;
        void capture_event(const core::status::Event::ptr &event) override;

    private:
        static const core::types::ValueMap<core::status::Level, DDS_Long> levelmap;

    private:
        RTI_DLOptions dl_options_;
        RTI_DLDistLogger *dist_logger_;
    };

}  // namespace core::dds
