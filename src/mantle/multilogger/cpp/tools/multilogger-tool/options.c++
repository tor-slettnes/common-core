// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Options parser for log tool
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "multilogger-grpc-client.h++"
#include "platform/path.h++"
#include "string/format.h++"

Options::Options()
    : Super(),
      signal_handle(core::platform::path->exec_name())
{
    this->describe("Send or receive log events to/from MultiLogger service");
}

void Options::add_options()
{
    Super::add_options();
    this->add_commands();
}

void Options::initialize()
{
    this->provider = multilogger::grpc::LogClient::create_shared(::options->host);
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
    auto min_level = core::str::convert_optional_to<core::status::Level>(
        this->next_arg(),
        core::status::Level::NONE);

    using namespace std::placeholders;
    multilogger::signal_log_item.connect(this->signal_handle,
                                         std::bind(&Options::on_log_item, this, _1));

    this->provider->start_listening({
        .min_level = min_level,
    });
}

void Options::on_monitor_end()
{
    this->provider->stop_listening();
    multilogger::signal_log_item.disconnect(this->signal_handle);
}

void Options::on_log_item(core::types::Loggable::ptr item)
{
    if (item)
    {
        std::cout << *item << std::endl;
    }
}

std::unique_ptr<Options> options;
