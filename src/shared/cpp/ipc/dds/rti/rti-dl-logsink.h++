/// -*- c++ -*-
//==============================================================================
/// @file rti-dl-logsink.h++
/// @brief A logger backed using RTI Distributed Logger infrastructure
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logging/sinks/messagesink.h++"
#include "types/valuemap.h++"
#include "types/create-shared.h++"

#include <rti_dl/rti_dl_cpp.h>
#include <ndds/ndds_cpp.h>
#include <dds/domain/DomainParticipant.hpp>

namespace shared::dds
{
    class RTIDistributedLogger : public shared::logging::MessageSink,
                                 public shared::types::enable_create_shared<RTIDistributedLogger>
    {
        using This = RTIDistributedLogger;
        using Super = shared::logging::MessageSink;

    protected:
        RTIDistributedLogger(const std::string &identity,
                             int domain_id);

    public:
        void set_threshold(shared::status::Level threshold) override;

    protected:
        void open() override;
        void close() override;
        void capture_message(const shared::logging::Message::Ref &msg) override;

    private:
        static const shared::types::ValueMap<shared::status::Level, DDS_Long> levelmap;

    private:
        RTI_DLOptions dl_options_;
        RTI_DLDistLogger *dist_logger_;
    };

}  // namespace shared::logging
