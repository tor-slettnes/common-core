// -*- c++ -*-
//==============================================================================
/// @file common.h++
/// @brief Parse command line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "argparse/parser.h++"
#include "logging/logging.h++"
#include "logging/sinks/logsink.h++"
#include "settings/settings.h++"

namespace core::argparse
{
    // Use `shared` scope for logging within this namespace
    use_shared_scope();

    //==========================================================================
    /// CommonOptions specialization with options for applications
    class CommonOptions : public Parser
    {
    public:
        /// Constructor that loads settings and builds options.
        CommonOptions(bool is_server);

        /// @brief
        ///     Parse command line options from main() and invoke associated
        ///     actions (create logger instances, daemonize, etc).
        void apply(int argc, char **argv);
        void apply(const ArgList &args);

        /// @brief
        ///     Parse input arguments provided in constructor and execute
        ///     associated actions (such as instantiating loggers and
        ///     daemonizing). May be overridden in subclasses.
        virtual void enact();

        /// @brief
        ///     Print a message on standard error and exit with a nonzero
        ///     status.
        void fail(const std::string &message,
                  bool show_hint = false,
                  std::ostream &out = std::cerr);

        /// @brief
        ///     Show help on standard output, then exit.
        void show_help_and_exit(const std::string &section = "ALL",
                                std::ostream &out = std::cout);

        /// @brief
        ///     Show help on standard output, then exit.
        void show_version_and_exit(std::ostream &out = std::cout);

    protected:
        /// @brief
        ///     Add options
        virtual void add_options();

        /// Add options related to logging
        virtual void add_log_options();

        /// Add options related to logging in a specific scope
        virtual void add_log_scope_options();

        /// Register available loggers
        virtual void register_loggers();

        std::optional<status::Level> level_setting(
            const std::string &option,
            const types::KeyValueMap &root = *core::settings);

        status::Level level_setting(
            const std::string &option,
            const types::KeyValueMap &root,
            status::Level fallback);

        types::KeyValueMap logsink_setting(
            const std::string &key);

        bool logsink_setting_enabled(
            const std::string &key,
            bool fallback = false);

        void set_threshold_from_config(const std::shared_ptr<logging::LogSink> &sink,
                                       const types::KeyValueMap &config);

    public:
        bool is_server;
        std::string command;
        std::string identity;
        uint domain_id;
        bool log_colors;
        bool log_context;
        bool log_to_syslog;
        bool log_to_stdout;
        bool log_to_stderr;
        bool log_to_json;
        bool log_to_file;
    };

    using RegisterLoggerFunction = std::function<void(Parser *)>;
    extern std::unordered_map<LongOpt, RegisterLoggerFunction> add_logger_methods;

}  // namespace core::argparse
