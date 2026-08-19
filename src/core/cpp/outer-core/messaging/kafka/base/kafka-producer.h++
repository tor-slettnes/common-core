/// -*- c++ -*-
//==============================================================================
/// @file kafka-producer.h++
/// @brief Wrapper for Kafka producer
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "kafka-endpoint.h++"

#include <thread>

namespace cc::kafka
{
    class ProducerBase : public Endpoint
    {
        using This = ProducerBase;
        using Super = Endpoint;

    protected:
        ProducerBase(const std::string &profile_name,
                     const core::types::KeyValueMap &settings = {});
        ~ProducerBase();

    public:
        void set_dr_callback(const DeliveryReportCapture::Callback &callback);
        void initialize() override;
        void deinitialize() override;

    protected:
        void init_dr_capture();
        void init_handle();
        RdKafka::Producer *handle() override;

    private:
        void start_poll();
        void stop_poll();
        void poll_worker();

    protected:
        void set_producer_key(const std::optional<std::string> &key);
        const std::optional<std::string> &producer_key() const;

        void produce(
            const std::string &topic,
            const core::types::Bytes &payload,
            const std::optional<core::dt::TimePoint> &timepoint = {},
            const std::optional<std::string_view> &key = {},
            const HeaderMap &headers = {},
            const DeliveryReportCapture::CallbackData::ptr &cb_data = {});

    private:
        void shutdown();

    private:
        RdKafka::Producer *producer_handle_;
        core::dt::Duration shutdown_timeout_;
        std::thread poll_thread_;
        bool keep_polling_;
        std::optional<std::string> producer_key_;
        DeliveryReportCapture dr_capture_;
    };
}  // namespace cc::kafka
