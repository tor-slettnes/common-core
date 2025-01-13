/// -*- c++ -*-
//==============================================================================
/// @file kafka-producer.h++
/// @brief Wrapper for Kafka producer
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "kafka-endpoint.h++"

namespace core::kafka
{
    class Producer : public Endpoint
    {
        using This = Producer;
        using Super = Endpoint;

    protected:
        Producer(const std::string &service_name);
        ~Producer();

    public:
        RdKafka::Producer *handle() const override;

    private:
        RdKafka::Producer *create_handle() const;
        void shutdown();

    protected:
        void produce(const std::string_view &topic,
                     const types::Bytes &payload,
                     const std::optional<std::string_view> &key = {});

    private:
        RdKafka::Producer *producer_handle_;
        core::dt::Duration shutdown_timeout_;
    };
}  // namespace core::kafka
