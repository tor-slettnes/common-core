/// -*- c++ -*-
//==============================================================================
/// @file kafka-producer.h++
/// @brief Wrapper for Kafka producer
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "kafka-endpoint.h++"

#include <thread>

namespace core::kafka
{
    class Producer : public Endpoint
    {
        using This = Producer;
        using Super = Endpoint;

    protected:
        using HeaderMap = std::unordered_map<std::string, std::string>;

    protected:
        Producer(const std::string &service_name,
                 const std::string &profile_name = {},
                 const core::types::KeyValueMap &settings = {});
        ~Producer();

    protected:
        void initialize() override;
        void deinitialize() override;

    protected:
        void init_dr_capture();
        void init_handle();

    public:
        RdKafka::Producer *handle() const override;
        void set_dr_callback(const DeliveryReportCapture::Callback &callback);

    private:
        void start_poll();
        void stop_poll();
        void poll_worker();

    protected:
        void set_producer_key(const std::optional<std::string> &key);
        const std::optional<std::string> &producer_key() const;

        void produce(
            const std::string &topic,
            const types::Bytes &payload,
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
}  // namespace core::kafka
