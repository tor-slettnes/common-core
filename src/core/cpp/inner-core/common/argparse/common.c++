// -*- c++ -*-
//==============================================================================
/// @file common.c++
/// @brief Parse command line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "argparse/common.h++"
#include "settings/settings.h++"
#include "status/level.h++"
#include "logging/logging.h++"
#include "logging/sinks/streamsink.h++"
#include "logging/sinks/logfilesink.h++"
#include "logging/sinks/jsonfilesink.h++"
#include "logging/sinks/csvfilesink.h++"
#include "platform/logsink.h++"
#include "platform/path.h++"
#include "platform/runtime.h++"
#include "buildinfo.h"

#include <functional>

#define LOG_SINKS   "log sinks"
#define SYSLOG_SINK "syslog"
#define STDOUT_SINK "stdout"
#define STDERR_SINK "stderr"
#define FILE_SINK   "file"
#define JSON_SINK   "json"
#define CSV_SINK    "csv"

#define LOGSINK_ENABLED   "enabled"
#define LOGSINK_THRESHOLD "threshold"
#define LOGSINK_PATHSPEC  "name template"
#define LOGSINK_ROTATION  "rotation"
#define LOGSINK_LOCALTIME "local time"
#define LOGSINK_COLUMNS   "column names"
#define LOGSINK_COLSEP    "column separator"

#define DEFAULT_ENABLED   false
#define DEFAULT_THRESHOLD core::status::Level::NONE
#define DEFAULT_PATHSPEC  "{executable}-{isodate}-{hour}{minute}{zoneoffset}"
#define DEFAULT_LOCALTIME true
#define DEFAULT_COLSEP    ","
#define DEFAULT_ROTATION                  \
    {                                     \
        .unit = core::dt::TimeUnit::HOUR, \
        .count = 6                        \
    }

namespace core::argparse
{
    CommonOptions::CommonOptions(bool is_server)
        : Parser(),
          is_server(is_server)
    {
        this->add_arg<std::string>(
            platform::path->exec_name(true),    // argname
            "",                                 // helptext
            &this->command,                     // target
            platform::path->exec_name(false));  // defaultValue
    }

    void CommonOptions::apply(int argc, char **argv)
    {
        this->apply(ArgList(argv, argv + argc));
    }

    void CommonOptions::apply(const ArgList &args)
    {
        try
        {
            this->add_options();
            this->parse_args(args);
        }
        catch (const std::exception &e)
        {
            this->fail(e.what(), true);
        }
        this->enact();
    }

    void CommonOptions::enact()
    {
        this->register_loggers();
        logging::message_dispatcher.initialize();
    }

    void CommonOptions::fail(const std::string &message,
                             bool show_hint,
                             std::ostream &out)
    {
        str::format(out, "%s: %s\n", this->command, message);
        if (show_hint)
        {
            out << "Try '--help' for complete usage, "
                << "or '--help=list' to list specific help sections."
                << std::endl;
        }
        std::exit(EINVAL);
    }

    void CommonOptions::show_help_and_exit(const std::string &section,
                                           std::ostream &out)
    {
        try
        {
            this->help(section, out);
            std::exit(0);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
            std::exit(EINVAL);
        }
    }

    void CommonOptions::show_version_and_exit(std::ostream &out)
    {
        out << PROJECT_VERSION << std::endl;
        std::exit(0);
    }

    void CommonOptions::add_options()
    {
        this->add_opt<std::string>(
            {"-h", "--help"},
            "SECTION",
            "Print help section SECTION (default: %default). "
            "Use \"--help=list\" to print a list of help sections.",
            [&](const std::string &section) {
                this->show_help_and_exit(section);
            },
            "all",
            false);

        this->add_void(
            {"-V", "--version"},
            "Print version number and exit",
            [&] {
                this->show_version_and_exit();
            });

        this->add_opt<std::string>(
            {"--identity"},
            "IDENTITY",
            "A name to identify this application amongst its peers",
            &this->identity,
            platform::path->exec_name());

        this->add_log_options();

        this->add_opt(
            {"--domain-id"},
            "DOMAIN_ID",
            "DDS Domain",
            &this->domain_id,
            core::settings->get("dds domain id", 0).as_uint());
    }

    void CommonOptions::add_log_options()
    {
        bool interactive = platform::runtime->isatty(0);
        if (auto level = this->get_optional_level("log default"))
        {
            logging::default_threshold = level.value();
        }

#ifndef _WIN32
        this->add_flag(
            {"--log-colors"},
            "Use ANSI colors to indicate log level when logging to a TTY",
            &this->log_colors,
            core::settings->get("log colors", true).as_bool());
#endif

        this->add_flag(
            {"--log-context", "--context"},
            "Include context in log message: "
            "log scope, thread ID, source file, line number, and method name.",
            &this->log_context,
            core::settings->get("log context", false).as_bool());

        this->add_opt<status::Level>(
            {"--log-default"},
            "LEVEL",
            "Default minimum log level to capture [%default]. "
            "This threshold may be overridden for specific log scopes, including "
            "the \"global\" scope.",
            &logging::default_threshold);

        this->add_log_scope_options();

        this->add_opt<status::Level>(
            {"--log-all"},
            "THRESHOLD",
            "Apply the specified minimum threshold in all logging scopes. "
            "This overrides the default and any scope-specific thresholds.",
            [](status::Level level) {
                logging::set_universal_threshold(level);
            },
            this->get_optional_level("log all"));

        this->add_const(
            {"--trace"},
            "Shorthand for --log-default=TRACE.",
            &logging::default_threshold,
            status::Level::TRACE);

        this->add_const(
            {"--debug", "--verbose"},
            "Shorthand for --log-default=DEBUG.",
            &logging::default_threshold,
            status::Level::DEBUG);

        this->add_const(
            {"--info"},
            "Shorthand for --log-default=INFO.",
            &logging::default_threshold,
            status::Level::INFO);

        this->add_const(
            {"--notice", "--muted"},
            "Shorthand for --log-default=NOTICE.",
            &logging::default_threshold,
            status::Level::NOTICE);

        this->add_void(
            {"--log-none", "--quiet"},
            "Disable logging completely.  Identical to \"--log-all=NONE\"",
            std::bind(&logging::set_universal_threshold, status::Level::NONE));

        this->add_flag(
            {"--log-to-syslog", "--syslog"},
            "Log to syslog on UNIX, or Event Log on Windows. "
            "Enabled by default is standard input is not a terminal.",
            &this->log_to_syslog,
            this->logsink_setting_enabled(SYSLOG_SINK, !interactive));

        this->add_flag(
            {"--log-to-stdout", "--stdout"},
            "Log to standard output. Implicitly disables `--log-to-stderr`.",
            &this->log_to_stdout,
            this->logsink_setting_enabled(STDOUT_SINK, false));

        this->add_flag(
            {"--log-to-stderr", "--stderr"},
            "Log to standard error. Enabled by default if standard input is a terminal.",
            &this->log_to_stderr,
            this->logsink_setting_enabled(STDERR_SINK, interactive));

        this->add_flag(
            {"--log-to-file"},
            "Log messages to a plaintext `.log` file. ",
            &this->log_to_file,
            this->logsink_setting_enabled(FILE_SINK, false));

        this->add_flag(
            {"--log-to-json"},
            "Log to a JSON file; one JSON-formatted log entry per line.",
            &this->log_to_json,
            this->logsink_setting_enabled(JSON_SINK, false));

        this->add_flag(
            {"--log-to-csv"},
            "Log to a CSV file, with column headers matching plaintext log message fields.",
            &this->log_to_csv,
            this->logsink_setting_enabled(CSV_SINK, this->is_server));
    }

    void CommonOptions::add_log_scope_options()
    {
        types::KeyValueMap thresholds = core::settings->get("log thresholds").as_kvmap();
        for (auto &[name, scope] : logging::scopes)
        {
            std::string levelname("(default)");

            if (auto level = this->get_optional_level(name, thresholds))
            {
                scope->threshold = level.value();
                levelname = str::convert_from(level.value());
            }

            this->add_opt<status::Level>(
                {"--log-" + name},
                "LEVEL",
                format("Minimum threshold for the %s log scope [%s]", name, levelname),
                &scope->threshold);
        }
    }

    void CommonOptions::register_loggers()
    {
        if (this->log_to_syslog)
        {
            if (auto sink = platform::logsink.get_shared())
            {
                logging::message_dispatcher.add_sink(sink);
                sink->set_include_context(this->log_context);

                types::KeyValueMap sinkspec = this->logsink_setting(SYSLOG_SINK);
                if (auto threshold = this->get_optional_level(LOGSINK_THRESHOLD, sinkspec))
                {
                    sink->set_threshold(threshold.value());
                }
            }
        }

        if (this->log_to_stderr)
        {
            types::KeyValueMap sinkspec = this->logsink_setting(STDERR_SINK);

            if (auto sink = logging::StreamSink::create_shared(
                    STDERR_SINK,
                    this->get_threshold(sinkspec),
                    std::cerr))
            {
                logging::message_dispatcher.add_sink(sink);
                sink->set_include_context(this->log_context);
            }
        }
        else if (this->log_to_stdout)
        {
            types::KeyValueMap sinkspec = this->logsink_setting(STDOUT_SINK);

            if (auto sink = logging::StreamSink::create_shared(
                    STDOUT_SINK,
                    this->get_threshold(sinkspec),
                    std::cout))
            {
                logging::message_dispatcher.add_sink(sink);
                sink->set_include_context(this->log_context);
            }
        }

        if (this->log_to_file)
        {
            types::KeyValueMap sinkspec = this->logsink_setting(FILE_SINK);

            if (auto sink = logging::LogFileSink::create_shared(
                    FILE_SINK,
                    this->get_threshold(sinkspec),
                    sinkspec.get(LOGSINK_PATHSPEC, DEFAULT_PATHSPEC).as_string(),
                    this->get_rotation(sinkspec),
                    sinkspec.get(LOGSINK_LOCALTIME, DEFAULT_LOCALTIME).as_bool()))
            {
                logging::message_dispatcher.add_sink(sink);
                sink->set_include_context(this->log_context);
            }
        }

        if (this->log_to_json)
        {
            types::KeyValueMap sinkspec = this->logsink_setting(JSON_SINK);

            if (auto sink = logging::JsonFileSink::create_shared(
                    JSON_SINK,
                    this->get_threshold(sinkspec),
                    sinkspec.get(LOGSINK_PATHSPEC, DEFAULT_PATHSPEC).as_string(),
                    this->get_rotation(sinkspec),
                    sinkspec.get(LOGSINK_LOCALTIME, DEFAULT_LOCALTIME).as_bool()))
            {
                logging::message_dispatcher.add_sink(sink);
            }
        }

        if (this->log_to_csv)
        {
            types::KeyValueMap sinkspec = this->logsink_setting(CSV_SINK);

            if (auto sink = logging::CSVMessageSink::create_shared(
                    CSV_SINK,
                    this->get_threshold(sinkspec),
                    sinkspec.get(LOGSINK_COLUMNS).as_valuelist().filter_by_type<std::string>(),
                    sinkspec.get(LOGSINK_PATHSPEC, DEFAULT_PATHSPEC).as_string(),
                    this->get_rotation(sinkspec),
                    sinkspec.get(LOGSINK_LOCALTIME, DEFAULT_LOCALTIME).as_bool(),
                    sinkspec.get(LOGSINK_COLSEP, DEFAULT_COLSEP).as_string()))
            {
                logging::message_dispatcher.add_sink(sink);
            }
        }
    }

    types::KeyValueMap CommonOptions::logsink_setting(
        const std::string &sink_name) const
    {
        return core::settings->get(LOG_SINKS).get(sink_name).as_kvmap();
    }

    bool CommonOptions::logsink_setting_enabled(
        const std::string &sink_name,
        bool fallback) const
    {
        return this->logsink_setting(sink_name).get(LOGSINK_ENABLED, fallback).as_bool();
    }

    std::optional<status::Level> CommonOptions::get_optional_level(
        const std::string &option,
        const types::KeyValueMap &config) const
    {
        if (const types::Value &value = config.get(option))
        {
            try
            {
                return str::convert_to<status::Level>(value.as_string());
            }
            catch (...)
            {
            }
        }
        return {};
    }

    status::Level CommonOptions::get_threshold(
        const types::KeyValueMap &config) const
    {
        return this->get_optional_level(LOGSINK_THRESHOLD, config)
            .value_or(DEFAULT_THRESHOLD);
    }

    dt::DateTimeInterval CommonOptions::get_rotation(
        const types::KeyValueMap &config) const
    {
        return str::convert_to<dt::DateTimeInterval>(
            config.get(LOGSINK_ROTATION).as_string(),
            DEFAULT_ROTATION);
    }

}  // namespace core::argparse
