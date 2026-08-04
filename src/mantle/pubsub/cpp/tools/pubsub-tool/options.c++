// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Options parser for message tool
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "relay-zmq-publisher.h++"
#include "relay-zmq-subscriber.h++"
#include "relay-grpc-client.h++"
#include "platform/symbols.h++"
#include "parsers/json/reader.h++"
#include "parsers/json/writer.h++"
#include "string/format.h++"
#include "types/symbolmap.h++"
#include "status/exceptions.h++"

namespace cc::platform::pubsub
{
    core::types::SymbolMap<Transport> transport_map =
        {
            {Transport::GRPC, "gRPC"},
            {Transport::ZMQ, "ZMQ"},
    };

    std::ostream& operator<<(std::ostream& stream, Transport transport)
    {
        return transport_map.to_stream(stream, transport);
    }

    std::istream& operator>>(std::istream& stream, Transport& transport)
    {
        return transport_map.from_stream(stream, &transport);
    }

    Options::Options()
        : signal_handle(TYPE_NAME_FULL(This)),
          json_output_(false),
          verbose_(false),
          transport_(Transport::GRPC)
    {
        this->describe("Send or receive messages via Relay");
    }

    void Options::add_options()
    {
        Super::add_options();

        this->add_const<Transport>(
            {"--grpc"},
            "Publish/Subscribe over gRPC [default]",
            &this->transport_,
            Transport::GRPC);

        this->add_const<Transport>(
            {"--zmq"},
            "Publish/Subscribe over ZMQ",
            &this->transport_,
            Transport::ZMQ);

        this->add_flag(
            {"--verbose"},
            "Print extra detail where applicable",
            &this->verbose_,
            false);

        this->add_opt<fs::path>(
            {"--input"},
            "FILENAME",
            "Read input for \"publish\" command from a JSON file. ",
            &this->input_file_);

        this->add_const<bool>(
            {"--json"},
            "Format outputs as JSON text",
            &this->json_output_,
            true);

        this->add_commands();
    }

    void Options::add_commands()
    {
        this->add_command(
            "get_replay_policies",
            {},
            "List all replay policies currently in effect.",
            std::bind(&Options::get_replay_policies, this));

        this->add_command(
            "get_replay_policy",
            {"TOPIC"},
            "Show the current replay policy for the specified TOPIC, if any",
            std::bind(&Options::get_replay_policy, this));

        this->add_command(
            "enable_replay",
            {"TOPIC", "[MAPPING_KEYS ...]"},
            "Enable replay on TOPIC. Following this, the lastest message "
            "published on this topic is cached in memory and replayed to "
            "future subscribers. If one or more MAPPING_KEYS are provided, "
            "messages are mapped by the corresponding key/value attributes, "
            "and thus multiple messages may be replayed.",
            std::bind(&Options::enable_topic_replay, this));

        this->add_command(
            "disable_replay",
            {"TOPIC", "[MAPPING_KEYS ...]"},
            "Disable replay on TOPIC.",
            std::bind(&Options::disable_topic_replay, this));

        this->add_command(
            "clear_replay",
            {"TOPIC", "[MAPPING_KEYS ...]"},
            "Disable replay on TOPIC.",
            std::bind(&Options::clear_replay_policies, this));

        this->add_command(
            "replay",
            {"[TOPIC]"},
            "Replay the latest message(s) published on TOPIC. "
            "If no TOPIC is specified, replay all cached messages.",
            std::bind(&Options::replay, this));

        this->add_command(
            "collect_topics",
            {"[verbose]"},
            "Listen for message topics, printing only new ones as they appear.",
            std::bind(&Options::monitor_topics, this));

        this->add_command(
            "publish",
            {"TOPIC", "[", "VALUE", "|", "[KEY VALUE]", "...", "]"},
            "Build and publish a message on the specified TOPIC. "
            "\n\n"
            "If a single VALUE argument follows TOPIC, it is published as is. "
            "Alternatively, an even number of arguments are interpreted as "
            "KEY/VALUE pairs and used to construct a variant value map."
            "\n\n"
            "The type of each VALUE argument is inferred heuristically, "
            "with words such as `true`, `false` and `null` as well as "
            "numeric literals yielding the most appropriate type. "
            "To force a particular value type, use JSON syntax: "
            "string values in \"double quotes\", "
            "maps as {KEY: VALUE, ...} pairs within in curly braces, "
            "and [VALUE, ...] lists inside square brackets. "
            "Such values may need to be further escaped in the user's shell, "
            "for instance within single quotations marks: "
            "'{\"pi\": 3.141592653589793238}'."
            "\n\n"
            "Alternatively, use \"--input\" to read the payload from a JSON file.",
            std::bind(&Options::publish, this));

        this->add_command(
            "listen",
            {"[TOPIC] ..."},
            "Subscribe to and listen for messages on the specified topics. "
            "If no topics are given, subscribe to all messsages.",
            std::bind(&Options::monitor, this));
    }

    void Options::get_replay_policies()
    {
        for (const auto& [topic, policy] : this->relay_control()->get_replay_policies())
        {
            core::str::format(std::cout, "%24s: %s\n", topic, policy);
        }
    }

    void Options::get_replay_policy()
    {
        std::string topic = this->get_arg("TOPIC");
        if (auto policy = this->relay_control()->get_replay_policy(topic))
        {
            std::cout << policy.value()
                      << std::endl;
        }
    }

    void Options::enable_topic_replay()
    {
        std::string topic = this->get_arg("TOPIC");
        ReplayPolicy policy{
            .replay_latest = true,
            .mapping_keys = this->remaining_args(),
        };
        this->relay_control()->assign_replay_policy(topic, policy);
    }

    void Options::disable_topic_replay()
    {
        std::string topic = this->get_arg("TOPIC");
        this->relay_control()->unassign_replay_policy(topic);
    }

    void Options::clear_replay_policies()
    {
        this->relay_control()->clear_replay_policies();
    }

    void Options::replay()
    {
        Snapshot snapshot;
        if (auto topic = this->next_arg())
        {
            if (auto payloads = this->relay_control()->replay_topic(*topic))
            {
                snapshot.insert_or_assign(*topic, *payloads);
            }
        }
        else
        {
            snapshot = this->relay_control()->replay_all();
        }

        for (const auto& [topic, payloads] : snapshot)
        {
            for (const core::types::Value& payload : payloads)
            {
                this->on_message(topic, payload);
            }
        }
    }

    void Options::publish()
    {
        std::string topic = this->get_arg("topic");
        bool published = false;
        auto publisher = this->publisher();
        this->publisher()->initialize();
        std::exception_ptr eptr;

        try
        {
            if (!this->input_file_.empty())
            {
                core::types::Value value = core::json::reader.read_file(this->input_file_);
                publisher->publish(topic, value);
                published = true;
            }

            if (std::distance(this->current_arg, this->args.end()) == 1)
            {
                // Single argument; interpret as JSON
                std::string literal = this->get_arg("VALUE");
                core::types::Value value = core::types::Value::from_literal(literal);
                publisher->publish(topic, value);
                published = true;
            }

            else
            {
                core::types::TaggedValueList payload = this->get_tvlist(false);
                if (!payload.empty())
                {
                    publisher->publish(topic, payload);
                    published = true;
                }
            }

            if (!published)
            {
                std::cout << "Nothing was published." << std::endl;
            }
        }
        catch (...)
        {
            eptr = std::current_exception();
        }

        this->publisher()->deinitialize();

        if (eptr)
        {
            std::rethrow_exception(eptr);
        }
    }

    void Options::on_monitor_start()
    {
        pubsub::TopicSet topics;

        while (auto arg = this->next_arg())
        {
            topics.insert(*arg);
        }

        using namespace std::placeholders;
        this->subscriber()->initialize();
        this->subscriber()->subscribe(
            this->signal_handle,
            topics,
            [&](const Topic& topic, const core::types::Value& message) {
                this->on_message(topic, message);
            });
    }

    void Options::on_monitor_end()
    {
        this->subscriber()->unsubscribe(this->signal_handle);
        this->subscriber()->deinitialize();
    }

    void Options::on_message(
        const Topic& topic,
        const core::types::Value& payload) const
    {
        std::cout << "["
                  << topic
                  << "] ";

        if (this->json_output_)
        {
            std::cout << core::json::writer.encoded(payload);
        }
        else
        {
            std::cout << payload;
        }

        std::cout << std::endl;
    }

    void Options::monitor_topics()
    {
        this->on_topiclistener_start();
        try
        {
            std::cout << std::endl
                      << "### Collecting topics. Press ENTER to end. ###"
                      << std::endl
                      << std::endl;
            std::string dummy;
            std::getline(std::cin, dummy);
        }
        catch (...)
        {
            this->on_topiclistener_end();
            throw;
        }
        this->on_topiclistener_end();
    }

    void Options::on_topiclistener_start()
    {
        using namespace std::placeholders;
        this->subscriber()->initialize();
        this->subscriber()->subscribe(
            this->signal_handle,
            {},
            [&](const Topic& topic, const core::types::Value& message) {
                this->on_message_topic(topic, message);
            });
    }

    void Options::on_topiclistener_end()
    {
        this->subscriber()->unsubscribe(this->signal_handle);
        this->subscriber()->deinitialize();
    }

    void Options::on_message_topic(const std::string& topic,
                                   const core::types::Value& message)
    {
        if (!this->seen_topics_.count(topic))
        {
            this->seen_topics_.insert(topic);
            if (this->verbose_)
            {
                std::cout << "["
                          << topic
                          << "]: "
                          << message
                          << std::endl;
            }
            else
            {
                std::cout << topic << std::endl;
            }
        }
    }

    std::shared_ptr<pubsub::ControlInterface> Options::relay_control()
    {
        if (!this->relay_control_)
        {
            switch (this->transport_)
            {
            case Transport::GRPC:
                if (auto client = pubsub::grpc::Client::create_shared(this->host))
                {
                    this->relay_control_ = client;
                    this->subscriber_ = client;
                    this->publisher_ = client;
                }
                break;

            default:
                throwf(core::exception::InvalidArgument,
                       "Relay control functions are not available with %s transport",
                       this->transport_);
                break;
            }
        }
        return this->relay_control_;
    }

    std::shared_ptr<pubsub::Subscriber> Options::subscriber()
    {
        if (!this->subscriber_)
        {
            switch (this->transport_)
            {
            case Transport::ZMQ:
                this->subscriber_ = pubsub::zmq::Subscriber::create_shared(this->host);
                break;

            case Transport::GRPC:
                if (auto client = pubsub::grpc::Client::create_shared(this->host))
                {
                    this->relay_control_ = client;
                    this->subscriber_ = client;
                    this->publisher_ = client;
                }
                break;
            }
        }

        return this->subscriber_;
    }

    std::shared_ptr<pubsub::Publisher> Options::publisher()
    {
        if (!this->publisher_)
        {
            switch (this->transport_)
            {
            case Transport::ZMQ:
                this->publisher_ = pubsub::zmq::Publisher::create_shared(this->host);
                break;

            case Transport::GRPC:
                if (auto client = pubsub::grpc::Client::create_shared(this->host))
                {
                    this->relay_control_ = client;
                    this->subscriber_ = client;
                    this->publisher_ = client;
                }
                break;
            }
        }

        return this->publisher_;
    }

}  // namespace cc::platform::pubsub
