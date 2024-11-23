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
#include "logging/sinks/core-sinks.h++"
#include "platform/path.h++"
#include "platform/runtime.h++"
#include "buildinfo.h"

#include <functional>

namespace core::argparse
{
    //--------------------------------------------------------------------------
    // Constants

    constexpr auto SYSLOG_SINK = "syslog";
    constexpr auto STDOUT_SINK = "stdout";
    constexpr auto STDERR_SINK = "stderr";
    constexpr auto FILE_SINK = "file";
    constexpr auto JSON_SINK = "json";
    constexpr auto CSV_SINK = "csv";

    constexpr auto SETTING_LOGSINK_ENABLED = "enabled";
    constexpr auto DEFAULT_LOGSINK_ENABLED = false;

    //--------------------------------------------------------------------------
    // CommonOptions

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

        this->add_void(
            {"--log-none", "--quiet"},
            "Disable logging completely.  Identical to \"--log-all=NONE\"",
            std::bind(&logging::set_universal_threshold, status::Level::NONE));

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

        this->add_log_sinks();
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

    void CommonOptions::add_log_sinks()
    {
        std::set<std::string> consumed_sink_types;

        // First we add options to log to sinks with IDs corresponding to keys
        // from the "log sinks" section in settings. The settings for each sink
        // may include a "type" value, in which case the corresponding sink
        // factory is used.  Otherwise, the type is assumed to be the same as
        // the sink ID (e.g. "file").

        if (auto sink_map = core::settings->get("log sinks").get_kvmap())
        {
            for (const auto &[sink_id, sink_specs] : *sink_map)
            {
                std::string sink_type = sink_specs.get("type", sink_id).as_string();
                if (logging::SinkFactory *factory =
                        logging::sink_registry.get(sink_type, nullptr))
                {
                    this->add_log_sink_option(sink_id, factory, sink_specs.as_kvmap());
                    consumed_sink_types.insert(sink_type);
                }
            }
        }

        // We now add sink factories that wasn't yet mentioned, with sink IDs
        // matching the sink type name (e.g. "stderr").
        for (const auto &[sink_type, factory] : logging::sink_registry)
        {
            if (consumed_sink_types.count(sink_type) == 0)
            {
                this->add_log_sink_option(sink_type, factory, {});
            }
        }
    }

    void CommonOptions::add_log_sink_option(
        const std::string &sink_id,
        logging::SinkFactory *factory,
        const types::KeyValueMap &sink_settings)
    {
        this->add_flag(
            {str::format("--log-to-%s", sink_id)},
            factory->description(),
            [=](bool enabled) {
                if (enabled)
                {
                    logging::message_dispatcher.add_sink(
                        factory->create_sink(sink_id, sink_settings));
                }
            },
            factory->default_enabled(sink_settings));
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

}  // namespace core::argparse
