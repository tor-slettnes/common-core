/// -*- c++ -*-
//==============================================================================
/// @file kafka-endpoint.h++
/// @brief Common functionality wrappers for KAFKA endpoints
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "messaging-endpoint.h++"
#include "types/value.h++"

#include <librdkafka/rdkafkacpp.h>

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

    private:
        void init_conf();

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

    private:
        RdKafka::Conf *conf_;

    protected:
        std::unordered_set<RdKafka::Topic *> topics;
    };
}  // namespace core::kafka
