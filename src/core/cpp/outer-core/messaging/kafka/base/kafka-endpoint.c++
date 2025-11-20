/// -*- c++ -*-
//==============================================================================
/// @file kafka-endpoint.c++
/// @brief Common functionality wrappers for KAFKA endpoints
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "kafka-endpoint.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

namespace core::kafka
{
    Endpoint::Endpoint(const std::string &endpoint_type,
                       const std::string &profile_name,
                       const core::types::KeyValueMap &settings)
        : Super("Kafka", endpoint_type, profile_name),
          conf_(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL))
    {
        this->init_logging();
        this->init_conf(settings);
    }

    Endpoint::~Endpoint()
    {
        for (RdKafka::Topic *topic : this->topics)
        {
            delete topic;
        }

        delete this->conf_;
    }

    std::string Endpoint::profile_name() const
    {
        return this->channel_name();
    }

    void Endpoint::initialize()
    {
        Super::initialize();
    }

    void Endpoint::deinitialize()
    {
        Super::deinitialize();
    }

    void Endpoint::init_conf(const core::types::KeyValueMap &settings)
    {
        for (const auto &[key, value] : settings)
        {
            this->set_config(key, value.as_string());
        }
    }

    void Endpoint::init_logging()
    {
        this->set_config("event_cb", &this->log_capture_, "LogCapture()");
    }

    void Endpoint::set_server_address(const std::string &server_address)
    {
        this->set_config("bootstrap.servers", server_address);
    }

    RdKafka::Conf *Endpoint::conf() const
    {
        return this->conf_;
    }

    RdKafka::Conf *Endpoint::topic_conf() const
    {
        return RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    }

    RdKafka::Topic *Endpoint::add_topic(const std::string &name,
                                        const core::types::KeyValueMap &topic_settings)
    {
        RdKafka::Conf *conf = this->topic_conf();
        std::string errstr;
        for (const auto &[key, value] : topic_settings)
        {
            this->check(conf->set(key, value.as_string(), errstr),
                        key,
                        value.as_string(),
                        errstr);
        }
        return this->add_topic(name, conf);
    }

    RdKafka::Topic *Endpoint::add_topic(const std::string &name,
                                        const RdKafka::Conf *topic_conf)
    {
        std::string error_string;
        if (RdKafka::Topic *topic = RdKafka::Topic::create(
                this->handle(),
                name,
                topic_conf,
                error_string))
        {
            this->topics.insert(topic);
            return topic;
        }
        else
        {
            throw exception::Unavailable(error_string);
        }
    }

    RdKafka::Topic *Endpoint::find_topic(const std::string &name) const
    {
        for (RdKafka::Topic *candidate : this->topics)
        {
            if (candidate->name() == name)
            {
                return candidate;
            }
        }
        return nullptr;
    }

    void Endpoint::set_config(const std::string &key,
                              const std::string &value)
    {
        this->set_config<std::string>(key, value, value);
    }

    void Endpoint::check(RdKafka::ErrorCode code,
                         const core::types::KeyValueMap &attributes) const
    {
        if (code != RdKafka::ERR_NO_ERROR)
        {
            throwf_args(exception::RuntimeError,
                        ("Kafka error %d: %s", code, RdKafka::err2str(code)),
                        attributes);
        }
    }

    void Endpoint::check(RdKafka::Error *error,
                         const core::types::KeyValueMap &attributes) const
    {
        if (error != nullptr)
        {
            std::string message = core::str::format(
                "Kafka error %d: [%s] %s",
                error->code(),
                error->name(),
                error->str());

            delete error;
            throw exception::RuntimeError(message, attributes);
        }
    }

    void Endpoint::check(RdKafka::Conf::ConfResult result,
                         const std::string &key,
                         const std::string &value,
                         const std::string &errstr) const
    {
        switch (result)
        {
        case RdKafka::Conf::CONF_OK:
            logf_debug("Applied Kafka setting: %s = %r",
                       key,
                       value);
            break;

        case RdKafka::Conf::CONF_INVALID:
            logf_error("Invalid Kakfa setting: %s = %r: %s",
                       key,
                       value,
                       errstr);
            break;

        case RdKafka::Conf::CONF_UNKNOWN:
            logf_error("Unknown Kakfa setting: %s = %r: %s",
                       key,
                       value,
                       errstr);
            break;
        }
    }

}  // namespace core::kafka
