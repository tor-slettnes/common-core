/// -*- c++ -*-
//==============================================================================
/// @file kafka-endpoint.h++
/// @brief Common functionality wrappers for KAFKA endpoints
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "kafka-callbacks.h++"
#include "messaging-endpoint.h++"

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
                 const std::string &service_name,
                 const std::string &server_address = {});

        ~Endpoint();

    protected:
        void initialize() override;
        void deinitialize() override;

    protected:
        void init_conf();
        void init_logging();
        void set_server_address(const std::string &server_address);

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
        template <class T>
        void set_config(const std::string &key,
                        T &&value,
                        const std::string &value_legend)
        {
            std::string errstr;
            this->check(this->conf()->set(key, value, errstr),
                        key,
                        value_legend,
                        errstr);
        }

    private:
        LogCapture log_capture_;
        RdKafka::Conf *conf_;

    protected:
        std::unordered_set<RdKafka::Topic *> topics;
    };
}  // namespace core::kafka
