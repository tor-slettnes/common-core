/// -*- c++ -*-
//==============================================================================
/// @file dds-logsink.c++
/// @brief A logger backed using DDS
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dds-logsink.h++"
#include "translate-idl-eventtypes.h++"
#include "translate-idl-inline.h++"
#include "platform/symbols.h++"

namespace core::dds
{
    DDSLogger::DDSLogger(const std::string &channel_name, int domain_id)
        : Super(),
          Publisher(channel_name, domain_id)
    {
    }

    void DDSLogger::open()
    {
        Super::open();
        this->log_writer = this->create_writer<CC::Status::Event>(
            CC::Status::LOG_TOPIC,  // topic_name
            true,                   // reliable
            false);                 // sync_latest
    }

    void DDSLogger::close()
    {
        this->log_writer.reset();
        Super::close();
    }

    void DDSLogger::capture_event(const status::Event::ptr &event)
    {
        if (this->log_writer)
        {
            this->log_writer->write(core::idl::encoded_shared<CC::Status::Event>(event));
        }
    }

}  // namespace core::dds
