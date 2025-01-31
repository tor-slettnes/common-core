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
                       const std::string &service_name)
        : Super("Kafka", endpoint_type, service_name),
          conf_(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL))
    {
        this->init_conf();
        this->init_logging();
    }

    Endpoint::~Endpoint()
    {
        for (RdKafka::Topic *topic : this->topics)
        {
            delete topic;
        }
    }

    void Endpoint::initialize()
    {
        Super::initialize();
    }

    void Endpoint::deinitialize()
    {
        Super::deinitialize();
    }

    void Endpoint::init_conf()
    {
        if (auto kvmap = this->setting("conf").get_kvmap())
        {
            std::string errstr;
            for (const auto &[key, value] : *kvmap)
            {
                this->check(this->conf()->set(key, value.as_string(), errstr),
                            key,
                            value.as_string(),
                            errstr);
            }
        }
    }

    void Endpoint::init_logging()
    {
        std::string errstr;
        this->check(this->conf()->set("event_cb",
                                      static_cast<RdKafka::EventCb*>(&this->log_capture_),
                                      errstr),
                    "event_cb",
                    "LogCapture()",
                    errstr);
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

    void Endpoint::check(RdKafka::ErrorCode code)
    {
        if (code != RdKafka::ERR_NO_ERROR)
        {
            throwf(exception::Unavailable,
                   "Kafka error %d: %s",
                   code,
                   RdKafka::err2str(code));
        }
    }

    void Endpoint::check(RdKafka::Conf::ConfResult result,
                         const std::string &key,
                         const std::string &value,
                         const std::string &errstr)
    {
        switch (result)
        {
        case RdKafka::Conf::CONF_OK:
            logf_info("Applied Kafka config setting: %s = %r",
                      key,
                      value);
            break;

        case RdKafka::Conf::CONF_INVALID:
            logf_error("Invalid Kakfa config setting: %s = %r: %s",
                       key,
                       value,
                       errstr);
            break;

        case RdKafka::Conf::CONF_UNKNOWN:
            logf_error("Unknown Kakfa config setting: %s = %r: %s",
                       key,
                       value,
                       errstr);
            break;
        }
    }

}  // namespace core::kafka
