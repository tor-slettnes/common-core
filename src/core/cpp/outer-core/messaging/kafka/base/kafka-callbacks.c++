/// -*- c++ -*-
//==============================================================================
/// @file kafka-callbacks.c++
/// @brief Miscellaneous callbacks to handle Kafka events
/// @author Tor Slettnes
//==============================================================================

#include "kafka-callbacks.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"
#include "string/stream.h++"

#include <functional>

namespace core::kafka
{
    auto rdkafka_log_scope =
        core::logging::Scope::create("rdkafka", core::status::Level::NOTICE);

    //--------------------------------------------------------------------------
    // LogCapture

    void LogCapture::event_cb(RdKafka::Event &event)
    {
        core::status::Level level = This::level_map.get(
            event.severity(),
            status::Level::WARNING);

        auto msg = custom_log_message(
            level,                   // level
            rdkafka_log_scope,       // scope
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
        {RdKafka::Event::EVENT_SEVERITY_CRITICAL, core::status::Level::ERROR},
        {RdKafka::Event::EVENT_SEVERITY_ERROR, core::status::Level::WARNING},
        {RdKafka::Event::EVENT_SEVERITY_WARNING, core::status::Level::NOTICE},
        {RdKafka::Event::EVENT_SEVERITY_NOTICE, core::status::Level::INFO},
        {RdKafka::Event::EVENT_SEVERITY_INFO, core::status::Level::DEBUG},
        {RdKafka::Event::EVENT_SEVERITY_DEBUG, core::status::Level::TRACE},
    };

    //--------------------------------------------------------------------------
    // DeliveryReportCapture

    void DeliveryReportCapture::set_callback(const Callback &callback)
    {
        this->callback = callback;
    }

    void *DeliveryReportCapture::add_callback_data(const CallbackData::ptr &data)
    {
        std::scoped_lock lock(this->callback_mutex);
        void *key = data.get();
        this->callback_map.insert_or_assign(key, data);
        return key;
    }

    void DeliveryReportCapture::dr_cb(RdKafka::Message &message)
    {
        if (this->callback)
        {
            CallbackData::ptr callback_data;
            {
                std::scoped_lock lock(this->callback_mutex);
                if (auto nh = this->callback_map.extract(message.msg_opaque()))
                {
                    callback_data = nh.mapped();
                }
            }

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
                    core::dt::ms_to_timepoint(message.timestamp().timestamp));  // timepoint
            }
            else if (message.status() != RdKafka::Message::MSG_STATUS_PERSISTED)
            {
                error = std::make_shared<core::status::Error>(
                    "Possible message delivery failure",                        // text
                    core::status::Domain::SERVICE,                              // domain
                    "RdKafka"s,                                                 // origin
                    0,                                                          // code
                    This::status_name_map.to_string(message.status(), {}),      // symbol
                    core::status::Level::WARNING,                               // level
                    core::dt::ms_to_timepoint(message.timestamp().timestamp));  // timepoint
            }

            try
            {
                logf_trace("Kafka delivery report callback, status=%s, error=%s",
                           message.status(),
                           error);

                this->callback(callback_data, error);
            }
            catch (...)
            {
                log_error("Kafka callback failed: ", std::current_exception());
            }
        }
    }

    core::types::SymbolMap<RdKafka::Message::Status> DeliveryReportCapture::status_name_map = {
        {RdKafka::Message::MSG_STATUS_NOT_PERSISTED, "MSG_STATUS_NOT_PERSISTED"},
        {RdKafka::Message::MSG_STATUS_POSSIBLY_PERSISTED, "MSG_STATUS_POSSIBLY_PERSISTED"},
        {RdKafka::Message::MSG_STATUS_PERSISTED, "MSG_STATUS_PERSISTED"},
    };

}  // namespace core::kafka
