// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Options parser for relay tool
/// @author Tor Slettnes
//==============================================================================

#include "common-options.h++"
#include "platform/symbols.h++"
#include "parsers/json/reader.h++"

namespace relay
{
    CommonOptions::CommonOptions(const std::string &implementation)
        : Super(),
          implementation(implementation),
          signal_handle(TYPE_NAME_FULL(This))
    {
        this->describe("Send or receive messages via Relay");
    }

    void CommonOptions::add_options()
    {
        Super::add_options();
        this->add_commands();
    }

    void CommonOptions::add_commands()
    {
        this->add_command(
            "publish",
            {"TOPIC", "PAYLOAD"},
            "Publish a message. PAYLOAD should be a JSON string.",
            std::bind(&CommonOptions::publish, this));

        this->add_command(
            "listen",
            {"[TOPIC] ..."},
            "Subscribe to and listen for messages on the specified topics. "
            "If no topics are given, subscribe to all messsages.",
            std::bind(&CommonOptions::monitor, this));
    }

    void CommonOptions::publish()
    {
        std::string topic = this->get_arg("topic");
        std::string payload = this->get_arg("payload");
        core::types::Value value = core::json::reader.decoded(payload);
        this->publisher()->publish(topic, value);
    }

    void CommonOptions::on_monitor_start()
    {
        std::vector<std::string> topics;

        while (auto arg = this->next_arg())
        {
            topics.push_back(*arg);
        }

        this->subscriber()->subscribe(
            this->signal_handle,
            topics,
            This::on_message);
    }

    void CommonOptions::on_monitor_end()
    {
        this->subscriber()->unsubscribe(this->signal_handle);
    }

    void CommonOptions::on_message(
        const Topic &topic,
        const Payload &payload)
    {
        core::str::format(std::cout, "[%s] %s\n", topic, payload);
    }
}  // namespace relay
