/// -*- c++ -*-
//==============================================================================
/// @file kafka-consumer.h++
/// @brief Wrapper for Kafka consumer
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "kafka-endpoint.h++"

#include <thread>

namespace cc::kafka
{
    class Consumer : public Endpoint
    {
        using This = Consumer;
        using Super = Endpoint;

    protected:
        Consumer(const std::string &profile_name,
                 const core::types::KeyValueMap &settings = {});
        ~Consumer();

    public:
        void initialize() override;
        void deinitialize() override;

    protected:
        void init_dr_capture();
        void init_handle();
        RdKafka::KafkaConsumer *handle() override;

    public:
        void subscribe(const std::vector<std::string> &topics);
        void unsubscribe();

    private:
        void start_consuming();
        void stop_consuming();
        void consume_worker();

    protected:
        virtual void handle_message(RdKafka::Message *message);

    protected:
        void set_consumer_key(const std::optional<std::string> &key);
        const std::optional<std::string> &consumer_key() const;

    private:
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
