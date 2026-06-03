// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Options parser for message tool
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-publisher.h++"
#include "relay-subscriber.h++"
#include "argparse/command.h++"
#include "types/filesystem.h++"

namespace cc::platform::pubsub::grpc
{
    enum class Transport
    {
        GRPC,
        ZMQ
    };

    std::ostream &operator<<(std::ostream &stream, Transport transport);
    std::istream &operator>>(std::istream &stream, Transport &transport);

    class Options : public core::argparse::CommandOptions
    {
        using This = Options;
        using Super = core::argparse::CommandOptions;

    public:
        Options();

    private:
        void add_options() override;
        void add_commands();

        void publish();
        void on_monitor_start() override;
        void on_monitor_end() override;

        void on_message(
            const Topic &topic,
            const core::types::Value &payload) const;

    protected:
        std::shared_ptr<pubsub::Subscriber> subscriber();
        std::shared_ptr<pubsub::Publisher> publisher();

    private:
        const std::string implementation;
        const std::string signal_handle;
        Transport transport_;
        fs::path input_file_;
        bool json_output_;
        std::shared_ptr<pubsub::Subscriber> subscriber_;
        std::shared_ptr<pubsub::Publisher> publisher_;
    };
}  // namespace cc::platform::pubsub::grpc
