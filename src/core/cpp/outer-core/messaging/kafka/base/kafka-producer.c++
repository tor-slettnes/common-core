/// -*- c++ -*-
//==============================================================================
/// @file kafka-producer.h++
/// @brief Wrapper for Kafka producer
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "kafka-producer.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace core::kafka
{
    const auto SETTING_SHUTDOWN_TIMEOUT = "shutdown timeout";
    const auto DEFAULT_SHUTDOWN_TIMEOUT = 2.0;

    Producer::Producer(const std::string &service_name,
                       const std::string &profile_name,
                       const core::types::KeyValueMap &settings)
        : Super("Producer", service_name, profile_name, settings),
          producer_handle_(nullptr),
          shutdown_timeout_(
              this->setting(SETTING_SHUTDOWN_TIMEOUT, DEFAULT_SHUTDOWN_TIMEOUT)
                  .as_duration()),
          keep_polling_(false)
    {
    }

    Producer::~Producer()
    {
        this->shutdown();
        delete this->producer_handle_;
    }

    void Producer::initialize()
    {
        Super::initialize();
        this->init_dr_capture();
        this->init_handle();
        this->start_poll();
    }

    void Producer::deinitialize()
    {
        this->stop_poll();
        Super::deinitialize();
    }

    void Producer::init_dr_capture()
    {
        this->set_config("dr_cb", &this->dr_capture_, "DeliveryReportCapture()");
    }

    void Producer::init_handle()
    {
        std::string error_string;
        if (RdKafka::Producer *producer = RdKafka::Producer::create(
                this->conf(),
                error_string))
        {
            this->producer_handle_ = producer;
        }
        else
        {
            throw exception::Unavailable(error_string);
        }
    }

    RdKafka::Producer *Producer::handle() const
    {
        return this->producer_handle_;
    }

    void Producer::set_dr_callback(const DeliveryReportCapture::Callback &callback)
    {
        this->dr_capture_.set_callback(callback);
    }

    void Producer::start_poll()
    {
        if (!this->poll_thread_.joinable())
        {
            logf_info("Starting %s polling thread", *this);
            this->keep_polling_ = true;
            this->poll_thread_ = std::thread(&This::poll_worker, this);
        }
    }

    void Producer::stop_poll()
    {
        if (this->poll_thread_.joinable())
        {
            logf_info("Stopping %s polling thread", *this);
            this->keep_polling_ = false;
            this->poll_thread_.join();
        }
    }

    void Producer::poll_worker()
    {
        while (this->keep_polling_)
        {
            this->handle()->poll(1000);
        }
    }

    void Producer::set_producer_key(const std::optional<std::string> &key)
    {
        this->producer_key_ = key;
    }

    const std::optional<std::string> &Producer::producer_key() const
    {
        return this->producer_key_;
    }

    void Producer::produce(
        const std::string &topic,
        const types::Bytes &payload,
        const std::optional<core::dt::TimePoint> &timepoint,
        const std::optional<std::string_view> &key,
        const HeaderMap &headers,
        const DeliveryReportCapture::CallbackData::ptr &cb_data)
    {
        core::dt::TimePoint tp_ = timepoint.value_or(core::dt::Clock::now());

        std::optional<std::string_view> key_ = key;
        if (!key_)
        {
            key_ = this->producer_key();
        }

        RdKafka::Headers *headers_ = RdKafka::Headers::create();
        for (const auto &[key, value] : headers)
        {
            headers_->add(key, value);
        }

        RdKafka::ErrorCode error_code = this->handle()->produce(
            topic,                                              // topic_name
            RdKafka::Topic::PARTITION_UA,                       // partition
            RdKafka::Producer::RK_MSG_COPY,                     // msgflags
            const_cast<types::Byte *>(payload.data()),          // payload
            payload.size(),                                     // len
            key_ ? const_cast<char *>(key_->data()) : nullptr,  // key
            key_ ? key_->size() : 0,                            // key_len
            dt::to_milliseconds(tp_),                           // timestamp
            headers_,                                           // headers
            this->dr_capture_.add_callback_data(cb_data));      // msg_opaque

        if (error_code != RdKafka::ERR_NO_ERROR)
        {
            // Per Kafka doc: The \p headers will be freed/deleted if the
            // produce() call succeeds, or left untouched if produce() fails.
            delete headers_;
            this->check(
                error_code,
                {
                    {"profile", this->profile_name()},
                    {"topic", topic},
                });
        }
    }

    void Producer::shutdown()
    {
        if (this->handle())
        {
            this->handle()->flush(dt::to_milliseconds(this->shutdown_timeout_));
        }
    }
}  // namespace core::kafka
