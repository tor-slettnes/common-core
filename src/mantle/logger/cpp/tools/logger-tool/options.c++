// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Options parser for log tool
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "logger-grpc-client.h++"
#include "platform/path.h++"
#include "string/format.h++"

Options::Options()
    : Super(),
      signal_handle(core::platform::path->exec_name())
{
    this->describe("Send or receive log events to/from Logger service");
}

void Options::add_options()
{
    Super::add_options();
    this->add_commands();
}

void Options::initialize()
{
    this->provider = logger::grpc::LoggerClient::create_shared(
        ::options->identity,
        core::status::Level::INFO,
        ::options->host);
    this->provider->initialize();
}

void Options::deinitialize()
{
    if (this->provider)
    {
        this->provider->deinitialize();
        this->provider.reset();
    }
}

void Options::on_monitor_start()
{
    auto min_level = core::str::convert_to<core::status::Level>(
        this->next_arg(),
        core::status::Level::NONE);

    using namespace std::placeholders;
    logger::signal_log_event.connect(this->signal_handle,
                                     std::bind(&Options::on_log_event, this, _1));

    this->provider->start_listening({
        .min_level = min_level,
    });
}

void Options::on_monitor_end()
{
    this->provider->stop_listening();
    logger::signal_log_event.disconnect(this->signal_handle);
}

void Options::on_log_event(core::status::Event::ptr event)
{
    if (event)
    {
        core::str::format(std::cout,
                          "[%.0s] signal_log_event(%s)\n",
                          core::dt::Clock::now(),
                          *event);
    }
}

std::unique_ptr<Options> options;
