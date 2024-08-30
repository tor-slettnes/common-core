// -*- c++ -*-
//==============================================================================
/// @file command.c++
/// @brief Parse options for command line utilities
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "command.h++"
#include "status/exceptions.h++"

namespace core::argparse
{
    //==========================================================================
    // CommandOptions methods

    CommandOptions::CommandOptions()
        : ClientOptions(),
          use_exit_status(false),
          current_arg(this->args.begin())
    {
    }

    void CommandOptions::add_options()
    {
        Super::add_options();

        this->add_flag(
            {"--status"},
            "By default, boolean queries indicate their results by printing "
            "\"true\" or \"false\" on standard output. With this option "
            "the result is instead reported via the process exit status: "
            "normal exit (code 0) if true, failure (code -1) if false. "
            "Mainly for use in shell scripts.",
            &this->use_exit_status);

        this->add_arg(
            "COMMAND",
            "See available commands below.",
            &this->command);  // target

        this->add_arg(
            "ARGS",
            "Command arguments.",
            &this->args,  // target
            {0, 0});      // (min,max) number of repeats

        this->add_help_section(
            "commands",
            std::bind(&CommandOptions::help_commands, this, std::placeholders::_1));
    }

    void CommandOptions::enact()
    {
        this->current_arg = this->args.begin();
        Super::enact();
    }

    void CommandOptions::report_status_and_exit(bool success)
    {
        if (this->use_exit_status)
        {
            std::exit(success ? 0 : -1);
        }
        else
        {
            std::cout << std::boolalpha << success << std::endl;
            std::exit(0);
        }
    }

    std::optional<std::string> CommandOptions::next_arg()
    {
        if (this->current_arg != this->args.end())
        {
            return *this->current_arg++;
        }
        else
        {
            return {};
        }
    }

    std::string CommandOptions::get_arg(const std::string &what)
    {
        if (auto arg = this->next_arg())
        {
            return arg.value();
        }
        else
        {
            throwf(exception::MissingArgument,
                   "Required argument \"%s\" is missing",
                   what);
        }
    }

    types::TaggedValueList CommandOptions::get_tvlist(bool required)
    {
        if (required && (this->current_arg == this->args.end()))
        {
            throwf(exception::MissingArgument,
                   "At least one key/value pair is required");
        }

        types::TaggedValueList tvlist;
        while (this->current_arg != this->args.end())
        {
            std::string key = this->get_arg("key");
            std::string value = this->get_arg("value");
            tvlist.emplace_back(key, types::Value::from_literal(value));
        }
        return tvlist;
    }

    types::KeyValueMap CommandOptions::get_attributes(bool required)
    {
        return this->get_tvlist(required).as_kvmap();
    }

    void CommandOptions::get_flags(FlagMap *map, bool allow_leftovers)
    {
        while (this->current_arg != this->args.end())
        {
            try
            {
                map->at(*this->current_arg) = true;
                this->current_arg++;
            }
            catch (const std::out_of_range &)
            {
                if (allow_leftovers)
                {
                    return;
                }
                else
                {
                    throw exception::InvalidArgument("Invalid argument",
                                                     *this->current_arg);
                }
            }
        }
    }

    void CommandOptions::add_command(
        const std::string &command,
        const std::vector<std::string> &args,
        const std::string &description,
        const Handler &handler)
    {
        this->command_descriptions.emplace_back(command, args, description);
        this->handlers.insert_or_assign(command, handler);
    }

    void CommandOptions::help_all(std::ostream &out)
    {
        if (this->description.size())
        {
            this->help_description(out);
            out << std::endl;
        }
        this->help_usage(out);
        if (this->options.size())
        {
            out << std::endl;
            this->help_options(out);
        }
        if (this->command_descriptions.size())
        {
            out << std::endl;
            this->help_commands(out);
        }
    }

    void CommandOptions::help_commands(std::ostream &out)
    {
        out << "Commands: " << std::endl;

        std::string intro = "  ";
        size_t left_margin = intro.length() + 2;
        bool sep = false;
        for (const auto &[command, args, description] : this->command_descriptions)
        {
            if (sep)
            {
                out << std::endl;
            }
            sep = true;
            out << intro << command << " "
                << str::wrap(args,                                   // words
                             intro.length() + command.length() + 1,  // start_column
                             left_margin,                            // left_margin,
                             this->wrap_column)
                << str::wrap(description, 0, left_margin);
        }
    }

    CommandOptions::Handler CommandOptions::command_handler()
    {
        try
        {
            return this->handlers.at(this->command);
        }
        catch (const std::out_of_range &)
        {
            this->fail(str::format("Unknown command %s", this->command), true);
            return {};
        }
    }

    bool CommandOptions::handle_command()
    {
        if (Handler handler = this->command_handler())
        {
            return this->handle_command(handler);
        }
        else
        {
            return false;
        }
    }

    bool CommandOptions::handle_command(const Handler &handler)
    {
        try
        {
            this->initialize();
            handler();
            this->deinitialize();
            return true;
        }
        catch (...)
        {
            std::cerr << std::current_exception() << std::endl;
            return false;
        }
    }

    void CommandOptions::monitor()
    {
        this->on_monitor_start();
        try
        {
            std::cout << std::endl
                      << "### Listening for updates. Press ENTER to end. ###"
                      << std::endl
                      << std::endl;
            std::string dummy;
            std::getline(std::cin, dummy);
        }
        catch (...)
        {
            this->on_monitor_end();
            std::rethrow_exception(std::current_exception());
        }
        this->on_monitor_end();
    }

}  // namespace core::argparse
