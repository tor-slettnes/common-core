// -*- c++ -*-
//==============================================================================
/// @file common.c++
/// @brief Parse command line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "argparse/common.h++"
#include "config/settings.h++"
#include "status/level.h++"
#include "logging/logging.h++"
#include "logging/sinks/streamsink.h++"
#include "logging/sinks/logfilesink.h++"
#include "logging/sinks/jsonfilesink.h++"
#include "platform/logsink.h++"
#include "platform/path.h++"
#include "platform/runtime.h++"
#include "buildinfo.h"

#include <functional>

#define LOG_TO_SYSLOG_SINK "log to syslog"
#define LOG_TO_STDOUT_SINK "log to stdout"
#define LOG_TO_STDERR_SINK "log to stderr"
#define LOG_TO_FILE_SINK   "log to file"
#define LOG_TO_JSON_SINK   "log to json"

#define LOGSINK_ENABLED_SETTING   "enabled"
#define LOGSINK_THRESHOLD_SETTING "threshold"
#define LOGFILE_TEMPLATE_SETTING  "name template"
#define LOGFILE_ROTATION_SETTING  "rotation"
#define LOGFILE_LOCALTIME_SETTING "local time"

namespace shared::argparse
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
        throw std::terminate;
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
            shared::settings->get("dds domain id", 0).as_uint());
    }

    void CommonOptions::add_log_options()
    {
        bool interactive = platform::runtime->isatty(0);
        if (auto level = this->level_setting("log default"))
        {
            logging::default_threshold = level.value();
        }

#ifndef _WIN32
        this->add_flag(
            {"--log-colors"},
            "Use ANSI colors to indicate log level when logging to a TTY",
            &this->log_colors,
            shared::settings->get("log colors", true).as_bool());
#endif

        this->add_flag(
            {"--log-context", "--context"},
            "Include context in log message: "
            "log scope, thread ID, source file, line number, and method name.",
            &this->log_context,
            shared::settings->get("log context", false).as_bool());

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
            this->level_setting("log all"));

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
            this->logsink_setting_enabled(LOG_TO_SYSLOG_SINK, !interactive));

        this->add_flag(
            {"--log-to-stdout", "--stdout"},
            "Log to standard output. Implicitly disables `--log-stderr`.",
            &this->log_to_stdout,
            this->logsink_setting_enabled(LOG_TO_STDOUT_SINK, false));

        this->add_flag(
            {"--log-to-stderr", "--stderr"},
            "Log to standard error. Enabled by default if standard input is a terminal.",
            &this->log_to_stderr,
            this->logsink_setting_enabled(LOG_TO_STDERR_SINK, interactive));

        this->add_flag(
            {"--log-to-file"},
            "Log messages to a plaintext `.log` file"
            "Path, threshold, rotation interval etc, can be configured "
            "via the \"log file\" application setting.",
            &this->log_to_file,
            this->logsink_setting_enabled(LOG_TO_FILE_SINK, false));

        this->add_flag(
            {"--log-to-json"},
            "Log to a JSON file."
            "Path, threshold, rotation interval etc, can be configured "
            "via the \"log json\" application setting.",
            &this->log_to_json,
            this->logsink_setting_enabled(LOG_TO_JSON_SINK, this->is_server));
    }

    void CommonOptions::add_log_scope_options()
    {
        types::KeyValueMap thresholds = shared::settings->get("log thresholds").as_kvmap();
        for (auto &[name, scope] : logging::scopes)
        {
            std::string levelname("(default)");

            if (auto level = this->level_setting(name, thresholds))
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

                types::KeyValueMap config = this->logsink_setting(LOG_TO_SYSLOG_SINK);
                this->set_threshold_from_config(sink, config);
            }
        }

        if (this->log_to_stdout)
        {
            if (auto sink = logging::StreamSink::create_shared(std::cout))
            {
                logging::message_dispatcher.add_sink(sink);
                sink->set_include_context(this->log_context);

                types::KeyValueMap config = this->logsink_setting(LOG_TO_STDOUT_SINK);
                this->set_threshold_from_config(sink, config);
            }
        }
        else if (this->log_to_stderr)
        {
            if (auto sink = logging::StreamSink::create_shared(std::cerr))
            {
                logging::message_dispatcher.add_sink(sink);
                sink->set_include_context(this->log_context);

                types::KeyValueMap config = this->logsink_setting(LOG_TO_STDERR_SINK);
                this->set_threshold_from_config(sink, config);
            }
        }

        if (this->log_to_file)
        {
            types::KeyValueMap config = this->logsink_setting(LOG_TO_FILE_SINK);
            std::string name_template = config.get(LOGFILE_TEMPLATE_SETTING).as_string();
            dt::Duration interval = dt::to_duration(
                config.get(LOGFILE_ROTATION_SETTING, 6 * 60 * 60).as_double());

            if (auto sink = logging::LogFileSink::create_shared(name_template, interval))
            {
                logging::message_dispatcher.add_sink(sink);
                sink->set_include_context(this->log_context);
                this->set_threshold_from_config(sink, config);
            }
        }

        if (this->log_to_json)
        {
            types::KeyValueMap config = this->logsink_setting(LOG_TO_JSON_SINK);
            std::string name_template = config.get(LOGFILE_TEMPLATE_SETTING).as_string();
            dt::Duration interval = dt::to_duration(
                config.get(LOGFILE_ROTATION_SETTING, 6 * 60 * 60).as_double());

            if (auto sink = logging::JsonFileSink::create_shared(name_template, interval))
            {
                logging::message_dispatcher.add_sink(sink);
                this->set_threshold_from_config(sink, config);
            }
        }
    }

    std::optional<status::Level> CommonOptions::level_setting(
        const std::string &option,
        const types::KeyValueMap &levels)
    {
        if (types::Value value = levels.get(option))
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

    status::Level CommonOptions::level_setting(
        const std::string &option,
        const types::KeyValueMap &levels,
        status::Level fallback)
    {
        return this->level_setting(option, levels).value_or(fallback);
    }

    types::KeyValueMap CommonOptions::logsink_setting(
        const std::string &sink_name)
    {
        return shared::settings->get(sink_name).as_kvmap();
    }

    bool CommonOptions::logsink_setting_enabled(
        const std::string &sink_name,
        bool fallback)
    {
        return this->logsink_setting(sink_name).get(LOGSINK_ENABLED_SETTING, fallback).as_bool();
    }

    void CommonOptions::set_threshold_from_config(
        const std::shared_ptr<logging::LogSink> &sink,
        const types::KeyValueMap &config)
    {
        if (auto threshold = this->level_setting(LOGSINK_THRESHOLD_SETTING, config))
        {
            sink->set_threshold(threshold.value());
        }
    }

}  // namespace shared::argparse
