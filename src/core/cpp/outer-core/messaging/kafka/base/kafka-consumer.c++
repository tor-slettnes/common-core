/// -*- c++ -*-
//==============================================================================
/// @file kafka-consumer.c++
/// @brief Wrapper for Kafka consumer
/// @author Tor Slettnes
//==============================================================================

#include "kafka-consumer.h++"
#include "logging/logging.h++"
#include "chrono/date-time.h++"
#include "status/exceptions.h++"

namespace cc::kafka
{
    const auto SETTING_SHUTDOWN_TIMEOUT = "shutdown timeout";
    const auto DEFAULT_SHUTDOWN_TIMEOUT = 2.0;

    Consumer::Consumer(const std::string &profile_name,
                       const core::types::KeyValueMap &settings)
        : Super("Consumer", profile_name, settings),
          consumer_handle_(nullptr),
          shutdown_timeout_(
              this->setting(SETTING_SHUTDOWN_TIMEOUT, DEFAULT_SHUTDOWN_TIMEOUT)
                  .as_duration()),
          keep_consuming_(false)
    {
    }

    Consumer::~Consumer()
    {
        this->shutdown();
        delete this->consumer_handle_;
    }

    void Consumer::initialize()
    {
        Super::initialize();
        this->start_consuming();
    }

    void Consumer::deinitialize()
    {
        this->stop_consuming();
        Super::deinitialize();
    }

    void Consumer::init_handle()
    {
        std::string error_string;
        if (RdKafka::KafkaConsumer *consumer = RdKafka::KafkaConsumer::create(
                this->conf(),
                error_string))
        {
            this->consumer_handle_ = consumer;
        }
        else
        {
            throw core::exception::Unavailable(
                "Failed to create Kafka consumer: " + error_string);
        }
    }

    RdKafka::KafkaConsumer *Consumer::handle()
    {
        if (!this->consumer_handle_)
        {
            this->init_handle();
        }

        return this->consumer_handle_;
    }

    void Consumer::subscribe(
        const std::vector<std::string> &topics)
    {
        RdKafka::ErrorCode error_code = this->handle()->subscribe(topics);

        this->check(
            error_code,
            {
                {"profile", this->profile_name()},
                {"topics", core::types::ValueList::create_shared_from(topics)},
            });

        this->start_consuming();
    }

    void Consumer::unsubscribe()
    {
        this->stop_consuming();
        this->handle()->unsubscribe();
    }

    void Consumer::start_consuming()
    {
        if (!this->consumer_thread_.joinable())
        {
            logf_info("%s: starting consumer thread", *this);
            this->keep_consuming_ = true;
            this->consumer_thread_ = std::thread(&This::consume_worker, this);
        }
    }

    void Consumer::stop_consuming()
    {
        if (this->consumer_thread_.joinable())
        {
            logf_info("%s: stopping consumer thread", *this);
            this->keep_consuming_ = false;
            this->consumer_thread_.join();
        }
    }

    void Consumer::consume_worker()
    {
        while (this->keep_consuming_)
        {
            if (RdKafka::Message *message = this->handle()->consume(1000))
            {
                switch (message->err())
                {
                case RdKafka::ErrorCode::ERR_NO_ERROR:
                    this->handle_message(message);
                    delete message;
                    break;

                case RdKafka::ErrorCode::ERR__TIMED_OUT:
                case RdKafka::ErrorCode::ERR__MSG_TIMED_OUT:
                    delete message;
                    break;

                default:
                    delete message;
                    this->check(message->err(),
                                {
                                    {"profile", this->profile_name()},
                                });
                    break;
                }
            }
        }
    }

    void Consumer::handle_message(RdKafka::Message *message)
    {
        auto payload = core::types::ByteVector::from_pointer(
            message->msg_opaque(),
            message->len());

        logf_info("Received Kafka message, timestamp=%s, offset=%d, key=%r, len=%d: %s",
                  core::dt::ms_to_timepoint(message->timestamp().timestamp),
                  message->offset(),
                  message->key(),
                  message->len(),
                  payload.to_hex());
    }

    void Consumer::set_consumer_key(const std::optional<std::string> &key)
    {
        this->consumer_key_ = key;
    }

    const std::optional<std::string> &Consumer::consumer_key() const
    {
        return this->consumer_key_;
    }

    void Consumer::shutdown()
    {
        this->keep_consuming_ = false;

        if (this->consumer_handle_)
        {
            this->consumer_handle_->close();
        }
    }
}  // namespace cc::kafka
