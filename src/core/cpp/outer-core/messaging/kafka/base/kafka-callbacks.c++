/// -*- c++ -*-
//==============================================================================
/// @file kafka-callbacks.c++
/// @brief Miscellaneous callbacks to handle Kafka events
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "kafka-callbacks.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace core::kafka
{
    //--------------------------------------------------------------------------
    // LogCapture

    void LogCapture::event_cb(RdKafka::Event &event)
    {
        static auto log_scope = ::core::logging::Scope::create(
            "rdkafka",
            status::Level::DEBUG);

        core::status::Level level = This::level_map.get(
            event.severity(),
            status::Level::WARNING);

        auto msg = custom_log_message(
            level,                   // level
            log_scope,               // scope
            core::dt::Clock::now(),  // timepoint
            "",                      // path
            0,                       // lineno
            "");                     // function

        msg->add(event.str());
        msg->dispatch();
    }

    const LogCapture::LevelMap LogCapture::level_map = {
        {RdKafka::Event::EVENT_SEVERITY_EMERG, core::status::Level::FATAL},
        {RdKafka::Event::EVENT_SEVERITY_ALERT, core::status::Level::CRITICAL},
        {RdKafka::Event::EVENT_SEVERITY_CRITICAL, core::status::Level::CRITICAL},
        {RdKafka::Event::EVENT_SEVERITY_ERROR, core::status::Level::ERROR},
        {RdKafka::Event::EVENT_SEVERITY_WARNING, core::status::Level::WARNING},
        {RdKafka::Event::EVENT_SEVERITY_NOTICE, core::status::Level::NOTICE},
        {RdKafka::Event::EVENT_SEVERITY_INFO, core::status::Level::INFO},
        {RdKafka::Event::EVENT_SEVERITY_DEBUG, core::status::Level::DEBUG},
    };

    //--------------------------------------------------------------------------
    // DeliveryReportCapture

    void DeliveryReportCapture::dr_cb(RdKafka::Message &message)
    {
        if (const auto *callback = reinterpret_cast<const Callback *>(message.msg_opaque()))
        {
            core::status::Error::ptr error;

            if (message.err() != RdKafka::ERR_NO_ERROR)
            {
                error = std::make_shared<core::status::Error>(
                    message.errstr(),                                           // text
                    core::status::Domain::SERVICE,                              // domain
                    "RdKafka"s,                                                 // origin
                    message.err(),                                              // code
                    RdKafka::err2str(message.err()),                            // symbol
                    core::status::Level::ERROR,                                 // level
                    core::dt::ms_to_timepoint(message.timestamp().timestamp));  // timepoint,
            }

            try
            {
                logf_debug("Kafka delivery report callback, status=%s, error=%s",
                           message.status(),
                           error);
                (*callback)(error);
            }
            catch (...)
            {
                log_error("Kafka callback failed: ", std::current_exception());
            }
        }
    }

}  // namespace core::kafka
