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

#define DDS_SINK    "dds"
#define RTI_DL_SINK "rti-dl"

namespace core::dds
{
    void ServerOptions::add_log_options()
    {
        Super::add_log_options();

        this->add_flag(
            {"--log-to-dds"},
            "Enable logging over DDS at the specified threshold [Default: %default]",
            &this->log_to_dds,
            this->logsink_setting_enabled(DDS_SINK, false));

#ifdef USE_RTI_LOGGER
        this->add_flag(
            {"--log-to-dl"},
            "Enable logging via RTI Distributed Logger [Default: %default]",
            &this->log_to_dl,
            this->logsink_setting_enabled(DL_SINK, true));
#endif
    }

    void ServerOptions::register_loggers()
    {
        Super::register_loggers();

        if (this->log_to_dds)
        {
            core::logging::message_dispatcher.add_sink(
                DDSLogger::create_shared(
                    DDS_SINK,                               // sink_id
                    core::platform::path->exec_name(true),  // channel_name
                    this->domain_id));                      // domain
        }

#ifdef USE_RTI_LOGGER
        if (this->log_to_dl)
        {
            core::logging::message_dispatcher.add_sink(
                RTIDistributedLogger::create_shared(
                    RTI_DL_SINK,                            // sink_id
                    core::platform::path->exec_name(true),  // application_id
                    this->domain_id));                      // domain
        }
#endif
    }

}  // namespace core::dds
