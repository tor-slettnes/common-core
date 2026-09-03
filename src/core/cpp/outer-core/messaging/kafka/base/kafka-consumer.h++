/// -*- c++ -*-
//==============================================================================
/// @file kafka-consumer.h++
/// @brief Wrapper for Kafka consumer
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "kafka-endpoint.h++"

#include "chrono/date-time.h++"
#include "types/bytevector.h++"
#include "types/value.h++"

#include <thread>

namespace cc::kafka
{
    class ConsumerBase : public Endpoint
    {
        using This = ConsumerBase;
        using Super = Endpoint;

    protected:
        ConsumerBase(const std::string &profile_name,
                     const core::types::KeyValueMap &settings,
                     const std::string &client_id,
                     const std::optional<std::string> &group_id = {},
                     const std::optional<std::string> &reset_policy = {});
        ~ConsumerBase();

    public:
        void initialize() override;
        void deinitialize() override;

    protected:
        void init_consumer_properties(
            const std::string &client_id,
            const std::optional<std::string> &group_id,
            const std::optional<std::string> &reset_policy);

        void init_handle();
        RdKafka::KafkaConsumer *handle() override;

    public:
        void subscribe(const std::vector<std::string> &topics);
        void clear_subscriptions();

    private:
        void start_consuming();
        void stop_consuming();
        void consume_worker();

        void handle_kafka_message(RdKafka::Message *message);
        void try_handle_message(
            const core::dt::TimePoint &tp,
            const std::string &topic,
            const std::string &key,
            const HeaderMap &header,
            const core::types::ByteVector &payload);

    protected:
        virtual void handle_message(
            const core::dt::TimePoint &tp,
            const std::string &topic,
            const std::string &key,
            const HeaderMap &header,
            const core::types::ByteVector &payload) = 0;

    protected:
        void set_consumer_key(const std::optional<std::string> &key);
        const std::optional<std::string> &consumer_key() const;

    private:
        HeaderMap extract_headers(
            const RdKafka::Headers *headers) const;

        void shutdown();

    private:
        RdKafka::KafkaConsumer *consumer_handle_;
        core::dt::Duration shutdown_timeout_;
        std::thread consumer_thread_;
        bool keep_consuming_;
        std::optional<std::string> consumer_key_;
        DeliveryReportCapture dr_capture_;
    };
}  // namespace cc::kafka
