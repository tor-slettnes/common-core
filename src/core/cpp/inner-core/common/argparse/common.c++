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

    CommonOptions::CommonOptions()
        : Parser()
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
        logging::dispatcher.initialize();
    }

    void CommonOptions::fail(const std::string &message,
                             bool show_hint,
                             std::ostream &out)
    {
        str::format(out, "%s: %s\n", this->command, message);
        if (show_hint)
        {
            out << "Try '--help' for complete usage, "
                << "or '--help=list' to list available help sections."
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
        if (auto level = this->get_optional_level("log default"))
        {
            logging::Scope::set_default_threshold(level.value());
        }

        this->add_opt<status::Level>(
            {"--log-default"},
            "LEVEL",
            "Default minimum log level to capture [%default]. "
            "This threshold may be overridden for specific log scopes, including "
            "the \"global\" scope.",
            &logging::Scope::default_threshold);

        this->add_log_scope_options();

        this->add_opt<status::Level>(
            {"--log-all"},
            "THRESHOLD",
            "Apply the specified minimum threshold in all logging scopes. "
            "This overrides the default and any scope-specific thresholds.",
            logging::Scope::set_universal_threshold,
            this->get_optional_level("log all"));

        this->add_void(
            {"--log-none"},
            "Disable logging completely.  Identical to \"--log-all=NONE\"",
            std::bind(&logging::Scope::set_universal_threshold, status::Level::NONE));

        this->add_flag(
            {"--log-context", "--log-source-context"},
            "Prefix log messagees with source context: source file, line number, "
            "function name. Applicable only to message-based log sinks such as "
            "stdout/stderr, syslog, and plaintext .log files, and if specified, "
            "will then override the default setting for those sinks.",
            logging::MessageSink::set_all_include_context);

        this->add_opt<fs::path>(
            {"--log-folder"},
            {"FOLDER"},
            "Set default log folder [%default]",
            &logging::RotatingPath::default_root_folder,
            core::settings->get("log folder", platform::path->log_folder()).as_string());

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
        for (auto &[sink_id, customization] : logging::sink_registry)
        {
            this->add_log_sink_option(sink_id, customization);
        }

        if (auto customization = logging::sink_registry.get(STDERR_SINK))
        {
            this->add_verbosity_options(customization);
        }
    }

    void CommonOptions::add_log_sink_option(
        const logging::SinkID &sink_id,
        const std::shared_ptr<logging::SinkCustomization> &customization)
    {
        this->add_opt<status::Level>(
            {str::format("--log-to-%s", sink_id)},
            "THRESHOLD",
            customization->factory->description(),
            [=](status::Level threshold) {
                customization->set_threshold(threshold);
            });
    }

    void CommonOptions::add_verbosity_options(
        const std::shared_ptr<logging::SinkCustomization> &customization)
    {
        this->add_void(
            {"--quiet"},
            str::format("Shorthand for --log-to-%s=%s",
                        customization->sink_id,
                        str::tolower(str::convert_from(status::Level::NONE))),
            [=] {
                customization->set_threshold(status::Level::NONE);
            });

        for (status::Level level : {status::Level::TRACE,
                                    status::Level::DEBUG,
                                    status::Level::INFO,
                                    status::Level::NOTICE,
                                    status::Level::WARNING})
        {
            this->add_void(
                {str::format("--%s", str::tolower(str::convert_from(level)))},
                str::format("Shorthand for --log-to-%s=%s",
                            customization->sink_id,
                            level),
                [=] {
                    customization->set_threshold(level);
                });
        }
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
