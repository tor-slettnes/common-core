/// -*- c++ -*-
//==============================================================================
/// @file kafka-callbacks.h++
/// @brief Miscellaneous callbacks to handle Kafka events
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/valuemap.h++"
#include "logging/logging.h++"
#include "status/error.h++"

#include <librdkafka/rdkafkacpp.h>
#include <mutex>

namespace core::kafka
{
    define_log_scope("rdkafka", status::Level::NOTICE);

    //--------------------------------------------------------------------------
    // LogCapture

    class LogCapture : public RdKafka::EventCb
    {
        using This = LogCapture;
        using LevelMap = core::types::ValueMap<RdKafka::Event::Severity, core::status::Level>;

    protected:
        void event_cb(RdKafka::Event &event) override;

    private:
        static const LevelMap level_map;
    };

    //--------------------------------------------------------------------------
    /// @class DeliveryReportCapture
    /// @brief
    ///     Capture delivery reports from Kafka producer

    class DeliveryReportCapture : public RdKafka::DeliveryReportCb
    {
        using This = DeliveryReportCapture;

    public:
        /// @brief
        ///     Abstract/Overridable data container passed from produce() call
        ///     back to callback handler.
        struct CallbackData
        {
            using ptr = std::shared_ptr<CallbackData>;

            virtual ~CallbackData() = default;
        };

    public:
        using Callback = std::function<void(
            CallbackData::ptr callback_data,
            core::status::Error::ptr error)>;

    public:
        void set_callback(const Callback &callback);

        void *add_callback_data(const CallbackData::ptr &data);

    protected:
        void dr_cb(RdKafka::Message &message) override;

    private:
        Callback callback;

    private:
        std::unordered_map<void *, CallbackData::ptr> callback_map;
        std::mutex callback_mutex;
    };
}  // namespace core::kafka
