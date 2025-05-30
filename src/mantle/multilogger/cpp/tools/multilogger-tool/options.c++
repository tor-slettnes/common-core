// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Options parser for log tool
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "multilogger-grpc-client.h++"
#include "platform/path.h++"
#include "logging/dispatchers/dispatcher.h++"
#include "logging/sinks/factory.h++"
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

    this->add_opt(
        {"--host"},
        "HOST",
        "Print only messages from matching host(s). "
        "This option may be repeated.",
        &this->hosts,
        This::ZeroOrMore);

    this->add_opt(
        {"--application", "--app"},
        "APPLICATION",
        "Print only messages from matching application(s). "
        "This option may be repeated.",
        &this->apps,
        This::ZeroOrMore);

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
        core::status::Level::TRACE);

    this->open_stream_sink(min_level);

    using namespace std::placeholders;
    multilogger::signal_log_item.connect(this->signal_handle,
                                         std::bind(&Options::on_log_item, this, _1));

    this->provider->start_listening({
        .min_level = min_level,
        .hosts = std::set(this->hosts.begin(), this->hosts.end()),
        .applications = std::set(this->apps.begin(), this->apps.end()),
    });
}

void Options::on_monitor_end()
{
    this->provider->stop_listening();
    this->close_stream_sink();

    multilogger::signal_log_item.disconnect(this->signal_handle);
}

void Options::open_stream_sink(core::status::Level threshold)
{
    if (auto customization = core::logging::sink_registry.get("stderr"))
    {
        customization->set_threshold(threshold);
        if (auto sink = customization->activate())
        {
            core::logging::dispatcher.add_sink(sink);
            this->stream_sink = sink;
        }
    }
}

void Options::close_stream_sink()
{
    core::logging::dispatcher.remove_sink(this->stream_sink);
    this->stream_sink.reset();
}


void Options::on_log_item(core::types::Loggable::ptr item)
{
    if (item && this->stream_sink)
    {
        this->stream_sink->capture(item);
    }
}

std::unique_ptr<Options> options;
