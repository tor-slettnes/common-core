/// -*- c++ -*-
//==============================================================================
/// @file dds-options-server.c++
/// @brief An option parser with DDS specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dds-options-server.h++"
#include "platform/path.h++"
#include "dds-logsink.h++"

#ifdef USE_RTI_LOGGER
#include "rti-dl-logsink.h++"
#endif

#define LOG_TO_DDS_SINK "log to dds"
#define LOG_TO_DL_SINK  "log to distributed logger"

namespace core::dds
{
    void ServerOptions::add_log_options()
    {
        Super::add_log_options();

        this->add_flag(
            {"--log-to-dds"},
            "Enable logging over DDS at the specified threshold [Default: %default]",
            &this->log_to_dds,
            this->logsink_setting_enabled(LOG_TO_DDS_SINK, false));

#ifdef USE_RTI_LOGGER
        this->add_flag(
            {"--log-to-dl"},
            "Enable logging via RTI Distributed Logger [Default: %default]",
            &this->log_to_dl,
            this->logsink_setting_enabled(LOG_TO_DL_SINK, true));
#endif
    }

    void ServerOptions::register_loggers()
    {
        Super::register_loggers();
        if (this->log_to_dds)
        {
            if (auto sink = DDSLogger::create_shared(
                    core::platform::path->exec_name(true),  // identity
                    this->domain_id))
            {
                core::logging::message_dispatcher.add_sink(sink);
                sink->set_include_context(this->log_context);

                core::types::KeyValueMap config = this->logsink_setting(LOG_TO_DDS_SINK);
                this->set_threshold_from_config(sink, config);
            }
        }

#ifdef USE_RTI_LOGGER
        if (this->log_to_dl)
        {
            if (auto sink = RTIDistributedLogger::create_shared(
                    core::platform::path->exec_name(true),  // identity
                    this->domain_id))
            {
                core::logging::message_dispatcher.add_sink(sink);
                sink->set_include_source(this->log_source);

                core::types::KeyValueMap config = this->logsink_setting(LOG_TO_DL_SINK);
                this->set_threshold_from_config(sink, config);
            }
        }
#endif
    }

}  // namespace core::dds
