/// -*- c++ -*-
//==============================================================================
/// @file kafka-endpoint.h++
/// @brief Common functionality wrappers for KAFKA endpoints
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "kafka-callbacks.h++"
#include "messaging-endpoint.h++"
#include "types/value.h++"

#include <librdkafka/rdkafkacpp.h>

#include <thread>

namespace core::kafka
{
    class Endpoint : public messaging::Endpoint
    {
        using This = Endpoint;
        using Super = messaging::Endpoint;

    protected:
        Endpoint(const std::string &endpoint_type,
                 const std::string &service_name);

        ~Endpoint();

    protected:
        void initialize() override;
        void deinitialize() override;

    private:
        void init_conf();
        void init_logging();

    public:
        virtual RdKafka::Handle *handle() const = 0;

        RdKafka::Conf *conf() const;
        RdKafka::Conf *topic_conf() const;

        RdKafka::Topic *add_topic(
            const std::string &name,
            const RdKafka::Conf *topic_conf);

        RdKafka::Topic *find_topic(
            const std::string &name) const;

    protected:
        static void check(RdKafka::ErrorCode code);
        static void check(RdKafka::Conf::ConfResult result,
                          const std::string &key,
                          const std::string &value,
                          const std::string &errstr);

    private:
        LogCapture log_capture_;
        RdKafka::Conf *conf_;

    protected:
        std::unordered_set<RdKafka::Topic *> topics;
    };
}  // namespace core::kafka
