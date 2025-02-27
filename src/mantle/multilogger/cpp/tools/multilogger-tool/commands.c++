// -*- c++ -*-
//==============================================================================
/// @file commands.c++
/// @brief Options parser for log tool - commmand handlers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "logging/logging.h++"

void Options::add_commands()
{
    this->add_command(
        "submit",
        {"TEXT", "[LEVEL]"},
        "Log a message at the specified level. [%default]. "
        "If no level is provided, use the default log level "
        "(as set with `--log-default` or its aliases)",
        std::bind(&Options::submit, this));

    this->add_command(
        "add_sink",
        {"SINK_ID", "TYPE", "[MIN_LEVEL]", "[PATH_TEMPLATE]"},
        "Add a multilogger sink",
        std::bind(&Options::add_sink, this));

    this->add_command(
        "remove_sink",
        {"SINK_ID"},
        "Remove a multilogger sink",
        std::bind(&Options::remove_sink, this));

    this->add_command(
        "get_sink",
        {"SINK_ID"},
        "Return specifications for an existing multilogger sink.",
        std::bind(&Options::get_sink, this));

    this->add_command(
        "get_all_sinks",
        {},
        "Return specifications for all currently registereed log sinks.",
        std::bind(&Options::get_all_sinks, this));

    this->add_command(
        "list_sinks",
        {},
        "List currently reistered log sinks.",
        std::bind(&Options::list_sinks, this));

    this->add_command(
        "list_message_fields",
        {},
        "List data fields/columns present in log messages.",
        std::bind(&Options::list_message_fields, this));

    this->add_command(
        "list_message_fields",
        {},
        "List data fields/columns present in logged errors.",
        std::bind(&Options::list_error_fields, this));

    this->add_command(
        "listen",
        {"[MIN_LEVEL]"},
        "Listen for log events with a severity level equal to or higher than the specified threshold",
        std::bind(&Options::monitor, this));
}

void Options::submit()
{
    std::string text = this->get_arg("text");
    auto level = core::str::convert_optional_to<core::status::Level>(
        this->next_arg(),
        core::logging::Scope::default_threshold);

    auto log_scope = core::logging::Scope::create("logtool", level);
    auto message = DEFAULT_LOG_MESSAGE(level);
    message->add(text);
    this->provider->submit(message);
}

void Options::add_sink()
{
    std::string sink_id = this->get_arg("sink_id");
    auto sink_type = core::str::convert_to<multilogger::SinkType>(this->get_arg("sink_type"));
    auto min_level = core::str::convert_optional_to<core::status::Level>(
        this->next_arg(),
        core::status::Level::DEBUG);
    std::string fn_template = this->next_arg().value_or("");

    bool success = this->provider->add_sink({
        .sink_id = sink_id,
        .sink_type = sink_type,
        .contract_id = {},
        .min_level = min_level,
        .filename_template = fn_template,
    });
    this->report_status_and_exit(success);
}

void Options::remove_sink()
{
    std::string sink_id = this->get_arg("sink_id");
    bool success = this->provider->remove_sink(sink_id);
    this->report_status_and_exit(success);
}

void Options::get_sink()
{
    std::string sink_id = this->get_arg("sink_id");
    std::cout << this->provider->get_sink_spec(sink_id) << std::endl;
}

void Options::get_all_sinks()
{
    for (const multilogger::SinkSpec &spec : this->provider->get_all_sink_specs())
    {
        std::cout << spec << std::endl;
    }
}

void Options::list_sinks()
{
    for (const multilogger::SinkID &sink_id : this->provider->list_sinks())
    {
        std::cout << sink_id << std::endl;
    }
}

void Options::list_message_fields()
{
    std::cout << this->provider->list_message_fields() << std::endl;
}

void Options::list_error_fields()
{
    std::cout << this->provider->list_error_fields() << std::endl;
}
