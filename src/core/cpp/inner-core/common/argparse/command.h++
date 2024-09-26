// -*- c++ -*-
//==============================================================================
/// @file command.h++
/// @brief Parse options for command line utilities
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "client.h++"

namespace core::argparse
{
    //==========================================================================
    /// Option parser for command-line utilities with subcommands
    class CommandOptions : public ClientOptions
    {
        using Super = ClientOptions;
        using CommandDescription = std::tuple<
            std::string,               // command
            std::vector<std::string>,  // args
            std::string>;              // description

    public:
        using FlagMap = std::map<std::string, bool>;
        using Handler = std::function<void()>;

        CommandOptions();
        void add_options() override;
        void enact() override;

        void report_status_and_exit(bool success);
        std::optional<std::string> next_arg();
        std::string get_arg(const std::string &what);
        types::TaggedValueList get_tvlist(bool required);
        types::KeyValueMap get_attributes(bool required);
        void get_flags(FlagMap *map, bool allow_leftovers = false);

        void add_command(const std::string &command,
                         const std::vector<std::string> &args,
                         const std::string &description,
                         const Handler &handler);

        void help_all(std::ostream &out) override;
        void help_commands(std::ostream &out);

        Handler command_handler();
        bool handle_command();
        virtual bool handle_command(const Handler &handler);

        virtual void monitor();
        virtual void on_monitor_start() {}
        virtual void on_monitor_end() {}
        virtual void initialize() {}
        virtual void deinitialize() {}

    protected:
        template <class T>
        std::optional<T> get_from_map(const core::types::SymbolMap<T> &map)
        {
            std::string arg = this->get_arg(map.joined_symbols());
            if (const std::optional<T> &value = map.from_string(arg))
            {
                return value;
            }
            else
            {
                std::cerr << "Invalid argument (expected "
                          << map.joined_symbols()
                          << "): "
                          << arg
                          << std::endl;
                return {};
            }
        }

    protected:
        std::string command;
        std::vector<std::string> args;
        std::vector<std::string>::iterator current_arg;
        bool use_exit_status;
        std::unordered_map<std::string, Handler> handlers;
        std::vector<CommandDescription> command_descriptions;
    };

}  // namespace core::argparse
