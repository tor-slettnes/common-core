// -*- c++ -*-
//==============================================================================
/// @file command.h++
/// @brief Parse options for command line utilities
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "client.h++"
#include "chrono/date-time.h++"
#include "thread/signaltemplate.h++"

namespace cc::core::argparse
{
    //==========================================================================
    /// Option parser for command-line utilities with subcommands
    class CommandOptions : public ClientOptions
    {
        using This = CommandOptions;
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
        std::vector<std::string> remaining_args();
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

        static std::string keywords_legend(
            const std::vector<std::string> &alternatives);

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

        template <class T>
        void connect_to_print(signal::DataSignal<T> *signal)
        {
            signal->connect(
                this->signal_handle,
                [=](const T &data) {
                    core::str::format(std::cout,
                                      "[%.3s] %s(%s)\n",
                                      dt::Clock::now(),
                                      signal->name(),
                                      data);
                });
        }

        template <class T>
        void connect_to_print(signal::SharedDataSignal<T> *signal)
        {
            signal->connect(
                this->signal_handle,
                [=](const std::shared_ptr<T> &data) {
                    if (data)
                    {
                        core::str::format(std::cout,
                                          "[%.3s] %s(%s)\n",
                                          dt::Clock::now(),
                                          signal->name(),
                                          *data);
                    }
                });
        }

        template <class T>
        void connect_to_print(signal::MappingSignal<T> *signal)
        {
            signal->connect(
                this->signal_handle,
                [=](signal::MappingAction mapping_action,
                    const std::string &mapping_key,
                    const T &data) {
                    core::str::format(std::cout,
                                      "[%.3s] %s(%s, %r, %s)\n",
                                      dt::Clock::now(),
                                      signal->name(),
                                      mapping_action,
                                      mapping_key,
                                      data);
                });
        }

        template <class S>
        void disconnect_from_print(S *signal)
        {
            signal->disconnect(this->signal_handle);
        }

    protected:
        std::string signal_handle;
        bool use_exit_status;
        std::vector<std::string>::iterator current_arg;
        std::string command;
        std::vector<std::string> args;
        std::unordered_map<std::string, Handler> handlers;
        std::vector<CommandDescription> command_descriptions;
    };

}  // namespace cc::core::argparse
