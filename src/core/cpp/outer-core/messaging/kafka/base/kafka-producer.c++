/// -*- c++ -*-
//==============================================================================
/// @file kafka-producer.h++
/// @brief Wrapper for Kafka producer
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "kafka-producer.h++"
#include "status/exceptions.h++"

namespace core::kafka
{
    const auto SETTING_SHUTDOWN_TIMEOUT = "shutdown timeout";
    const auto DEFAULT_SHUTDOWN_TIMEOUT = 2.0;

    Producer::Producer(const std::string &service_name)
        : Super("Producer", service_name),
          producer_handle_(This::create_handle()),
          shutdown_timeout_(
              this->setting(SETTING_SHUTDOWN_TIMEOUT, DEFAULT_SHUTDOWN_TIMEOUT)
                  .as_duration())
    {
    }

    Producer::~Producer()
    {
        this->shutdown();
    }

    RdKafka::Producer *Producer::handle() const
    {
        return this->producer_handle_;
    }

    void Producer::shutdown()
    {
        if (this->handle())
        {
            this->handle()->flush(dt::to_milliseconds(this->shutdown_timeout_));
        }
    }

    RdKafka::Producer *Producer::create_handle() const
    {
        std::string error_string;
        if (RdKafka::Producer *producer = RdKafka::Producer::create(
                this->conf(),
                error_string))
        {
            return producer;
        }
        else
        {
            throw exception::Unavailable(error_string);
        }
    }

    void Producer::produce(const std::string_view &topic,
                           const types::Bytes &payload,
                           const std::optional<std::string_view> &key)
    {
        this->check(this->handle()->produce(
            std::string(topic, 0, topic.size()),              // topic_name
            RdKafka::Topic::PARTITION_UA,                     // partition
            RdKafka::Producer::RK_MSG_COPY,                   // msgflags
            const_cast<types::Byte *>(payload.data()),        // payload
            payload.size(),                                   // len
            key ? const_cast<char *>(key->data()) : nullptr,  // key
            key ? key->size() : 0,                            // key_len
            dt::to_milliseconds(dt::Clock::now()),            // timestamp
            nullptr));                                        // msg_opaque
    }

}  // namespace core::kafka
