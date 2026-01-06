// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Options parser for relay tool
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "argparse/command.h++"
#include "relay-publisher.h++"
#include "relay-subscriber.h++"
#include "thread/signaltemplate.h++"

namespace relay
{
    class CommonOptions : public core::argparse::CommandOptions
    {
        using This = CommonOptions;
        using Super = core::argparse::CommandOptions;

    public:
        CommonOptions(const std::string &implementation);

    protected:
        virtual std::shared_ptr<relay::Subscriber> subscriber() = 0;
        virtual std::shared_ptr<relay::Publisher> publisher() = 0;

    private:
        void add_options() override;
        void add_commands();

        void publish();
        void on_monitor_start() override;
        void on_monitor_end() override;

        static void on_message(
            const Topic &topic,
            const Payload &payload);

    private:
        const std::string implementation;
        const std::string signal_handle;
    };

}  // namespace relay
