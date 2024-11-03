/// -*- c++ -*-
//==============================================================================
/// @file rti-dl-logsink.c++
/// @brief A logger backed using RTI Distributed Logger infrastructure
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "rti-dl-logsink.h++"

namespace core::dds
{
    RTIDistributedLogger::RTIDistributedLogger(
        const std::string &sink_id,
        status::Level threshold,
        const std::string &application_id,
        int domain_id)
        : LogSink(sink_id, threshold),
          dist_logger_(nullptr)
    {
        this->dl_options_.setApplicationKind(application_id.c_str());
        this->dl_options_.setDomainId(domain_id);
        this->dl_options_.setEchoToStdout(false);
        this->set_threshold(this->threshold());
    }

    void RTIDistributedLogger::set_threshold(core::status::Level threshold)
    {
        Super::set_threshold(threshold);
        if (DDS_Long filterlevel = This::levelmap.get(threshold, 0))
        {
            this->dl_options_.setFilterLevel(filterlevel);
        }
    }

    void RTIDistributedLogger::open()
    {
        Super::open();
        if (!RTI_DLDistLogger::setOptions(this->dl_options_))
        {
            throw std::runtime_error("Failed to set options for DDS logging");
        }

        this->dist_logger_ = RTI_DLDistLogger::getInstance();
    }

    void RTIDistributedLogger::close()
    {
        this->dist_logger_->finalizeInstance();
        this->dist_logger_ = nullptr;
        Super::close();
    }

    void RTIDistributedLogger::capture_event(const core::status::Event::ptr &event)
    {
        if (this->dist_logger_)
        {
            if (const int *level = RTIDistributedLogger::levelmap.get_ptr(event->level()))
            {
                timespec ts = core::dt::to_timespec(event->timepoint());
                std::string text = this->formatted(event);
                std::string domain_name = core::status::DomainNames.to_string(event->domain(), "NONE");
                this->dist_logger_->logMessageWithParams({
                    *level,                             // log_level
                    text.c_str(),                       // message
                    domain_name.c_str(),                // category
                    {static_cast<DDS_Long>(ts.tv_sec),  // timestamp
                     static_cast<DDS_UnsignedLong>(ts.tv_nsec)},
                });
            }
        }
    }

    const types::ValueMap<status::Level, DDS_Long> RTIDistributedLogger::levelmap = {
        {core::status::Level::TRACE, 800},
        {core::status::Level::DEBUG, 700},
        {core::status::Level::INFO, 600},
        {core::status::Level::NOTICE, 500},
        {core::status::Level::WARNING, 400},
        {core::status::Level::FAILED, 300},
        {core::status::Level::CRITICAL, 200},
        {core::status::Level::FATAL, 100},
    };

}  // namespace core::dds
