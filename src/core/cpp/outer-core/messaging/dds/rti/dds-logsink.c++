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
    DDSLogger::DDSLogger(const std::string &sink_id,
                         const std::string &channel_name,
                         int domain_id)
        : MessageSink(sink_id),
          Publisher(channel_name, domain_id)
    {
    }

    void DDSLogger::open()
    {
        Super::open();
        this->log_writer = this->create_writer<CC::Status::LogMessage>(
            CC::Status::LOG_TOPIC,  // topic_name
            true,                   // reliable
            false);                 // sync_latest
    }

    void DDSLogger::close()
    {
        this->log_writer.reset();
        Super::close();
    }

    bool DDSLogger::handle_message(const logging::Message::ptr &message)
    {
        if (this->log_writer)
        {
            this->log_writer->write(
                idl::encoded_shared<CC::Status::LogMessage>(message));
            return true;
        }
        else
        {
            return false;
        }
    }
}  // namespace core::dds
