/// -*- c++ -*-
//==============================================================================
/// @file rti-dl-logsink.c++
/// @brief A logger backed using RTI Distributed Logger infrastructure
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "rti-dl-logsink.h++"

namespace cc::logging
{
    RTILogger::RTILogger(const std::string &identity,
                         int domain_id)
        : Super(),
          dist_logger_(nullptr)
    {
        this->options_.setApplicationKind(identity.c_str());
        this->options_.setDomainId(domain_id);
        this->options_.setEchoToStdout(false);
        this->set_threshold(this->threshold());
    }

    void RTILogger::set_threshold(status::Level threshold)
    {
        Super::set_threshold(threshold);
        if (DDS_Long filterlevel = This::levelmap.get(threshold, 0))
        {
            this->options_.setFilterLevel(filterlevel);
        }
    }

    void RTILogger::open()
    {
        Super::open();
        if (!RTI_DLDistLogger::setOptions(this->options_))
        {
            throw std::runtime_error("Failed to set options for DDS logging");
        }

        this->dist_logger_ = RTI_DLDistLogger::getInstance();
    }

    void RTILogger::close()
    {
        this->dist_logger_->finalizeInstance();
        this->dist_logger_ = nullptr;
        Super::close();
    }

    void RTILogger::capture_message(const Message::Ref &msg)
    {
        if (this->dist_logger_)
        {
            if (const int *level = RTILogger::levelmap.get_ptr(msg->level()))
            {
                timespec ts = cc::dt::to_timespec(msg->timepoint());
                this->dist_logger_->logMessageWithParams({
                    *level,                             // log_level
                    msg->text().c_str(),                // message
                    msg->scopename().c_str(),           // category
                    {static_cast<DDS_Long>(ts.tv_sec),  // timestamp
                     static_cast<DDS_UnsignedLong>(ts.tv_nsec)},
                });
            }
        }
    }

    const types::ValueMap<status::Level, DDS_Long> RTILogger::levelmap = {
        {status::Level::TRACE, 800},
        {status::Level::DEBUG, 700},
        {status::Level::INFO, 600},
        {status::Level::NOTICE, 500},
        {status::Level::WARNING, 400},
        {status::Level::FAILED, 300},
        {status::Level::CRITICAL, 200},
        {status::Level::FATAL, 100},
    };

}  // namespace cc::logging
