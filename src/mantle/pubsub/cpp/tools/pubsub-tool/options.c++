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
#include "types/symbolmap.h++"

namespace relay::grpc
{
    core::types::SymbolMap<Transport> transport_map =
        {
            {Transport::GRPC, "gRPC"},
            {Transport::ZMQ, "ZMQ"},
    };

    std::ostream &operator<<(std::ostream &stream, Transport transport)
    {
        return transport_map.to_stream(stream, transport);
    }

    std::istream &operator>>(std::istream &stream, Transport &transport)
    {
        return transport_map.from_stream(stream, &transport);
    }

    Options::Options()
        : signal_handle(TYPE_NAME_FULL(This)),
          transport_(Transport::GRPC),
          json_input_(false)

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

        this->add_opt<fs::path>(
            {"--input"},
            "FILENAME",
            "Read input for \"publish\" command from a JSON file. "
            "Unless the \"--json\" option is used, the file contents "
            "are published as plain text.",
            &this->input_file_);

        this->add_const<bool>(
            {"--text"},
            "Publish inputs as plain text [default]",
            &this->json_input_,
            false);

        this->add_const<bool>(
            {"--json"},
            "Parse inputs as JSON text",
            &this->json_input_,
            true);

        this->add_commands();
    }

    void Options::add_commands()
    {
        this->add_command(
            "publish",
            {"TOPIC", "[PAYLOAD] ... "},
            "Publish a message. "
            "Alternatively, use \"--input\" to read from a JSON file. "
            "Unless the \"--json\" option is used, the input is "
            "read and  published as plain text.",
            std::bind(&Options::publish, this));

        this->add_command(
            "listen",
            {"[TOPIC] ..."},
            "Subscribe to and listen for messages on the specified topics. "
            "If no topics are given, subscribe to all messsages.",
            std::bind(&Options::monitor, this));
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
            while (auto payload = this->next_arg())
            {
                core::types::Value value =
                    this->json_input_ ? core::json::reader.decoded(*payload)
                                      : core::types::Value(*payload);

                publisher->publish(topic, value);
                published = true;
            }

            if (!this->input_file_.empty())
            {
                core::types::Value value = core::json::reader.read_file(this->input_file_);
                publisher->publish(topic, value);
                published = true;
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
        relay::TopicSet topics;

        while (auto arg = this->next_arg())
        {
            topics.insert(*arg);
        }

        this->subscriber()->initialize();
        this->subscriber()->subscribe(
            this->signal_handle,
            topics,
            This::on_message);
    }

    void Options::on_monitor_end()
    {
        this->subscriber()->unsubscribe(this->signal_handle);
        this->subscriber()->deinitialize();
    }

    void Options::on_message(
        const Topic &topic,
        const Payload &payload)
    {
        core::str::format(std::cout, "[%s] %s\n", topic, payload);
    }

    std::shared_ptr<relay::Subscriber> Options::subscriber()
    {
        if (!this->subscriber_)
        {
            switch (this->transport_)
            {
            case Transport::ZMQ:
                this->subscriber_ = relay::zmq::Subscriber::create_shared(this->host);
                break;

            case Transport::GRPC:
                if (auto client = relay::grpc::Client::create_shared(this->host))
                {
                    this->subscriber_ = client;
                    this->publisher_ = client;
                }
                break;
            }
        }

        return this->subscriber_;
    }

    std::shared_ptr<relay::Publisher> Options::publisher()
    {
        if (!this->publisher_)
        {
            switch (this->transport_)
            {
            case Transport::ZMQ:
                this->publisher_ = relay::zmq::Publisher::create_shared(this->host);
                break;

            case Transport::GRPC:
                if (auto client = relay::grpc::Client::create_shared(this->host))
                {
                    this->subscriber_ = client;
                    this->publisher_ = client;
                }
                break;
            }
        }

        return this->publisher_;
    }

}  // namespace relay::grpc
