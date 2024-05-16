// -*- c++ -*-
//==============================================================================
/// @file parser.c++
/// @brief Parse command line arguments, help output
//==============================================================================

#include "parser.h++"
#include "string/misc.h++"  // str::join()
#include "status/exceptions.h++"
#include "platform/path.h++"

#include <regex>
#include <sstream>  // std::stringstream

namespace core::argparse
{
    //==========================================================================
    // Parser methods

    Parser::Parser(size_t align_column,
                   size_t wrap_column)
        : align_column(align_column),
          wrap_column(wrap_column)
    {
        using namespace std::placeholders;
        this->add_help_section(
            "all",
            std::bind(&Parser::help_all, this, _1));

        this->add_help_section(
            "list",
            std::bind(&Parser::help_section_list, this, _1));

        this->add_help_section(
            "usage",
            std::bind(&Parser::help_usage, this, _1));

        this->add_help_section(
            "options",
            std::bind(&Parser::help_options, this, _1));

        this->add_help_section(
            "description",
            std::bind(&Parser::help_description, this, _1));
    }

    void Parser::describe(const std::string &description)
    {
        this->description = description;
    }

    void Parser::add(const OptionPtr &option)
    {
        this->options.emplace_back(option);
    }

    void Parser::add_help_section(const std::string &section,
                                  const std::function<void(std::ostream &out)> &method)
    {
        this->help_sections.insert_or_assign(section, method);
    }

    void Parser::help_all(std::ostream &out)
    {
        this->help_usage(out);
        out << std::endl;
        this->help_options(out);
        out << std::endl;
        this->help_description(out);
    }

    void Parser::help_section_list(std::ostream &out)
    {
        out << "Available help sections (use --help=SECTION to show):" << std::endl;
        for (const auto &[section, method] : this->help_sections)
        {
            out << "    " << section << std::endl;
        }
    }

    void Parser::help_usage(std::ostream &out)
    {
        std::string intro = "Usage:";
        std::vector<std::string> words;
        bool has_misc_options = false;
        bool has_command = false;

        for (const OptionPtr &opt : this->options)
        {
            if (!has_command && opt->argname.has_value())
            {
                // Capture the command name in "intro" string to adjust the left
                // margin for the remainer of the syntax string.
                intro.append(" ").append(opt->argname.value());
                has_command = true;
            }
            else if (!opt->is_named() || (opt->repeats.first > 0 && !opt->has_default()))
            {
                // This is an unnamed or required option; include this separately.
                words.push_back(opt->get_syntax());
            }
            else if (!has_misc_options)
            {
                // This is an optional, named option. No need to include it seprately.
                words.emplace(words.begin(), "[OPTION ...]"s);
                has_misc_options = true;
            }
        }

        out << intro
            << str::wrap(words, (uint)intro.length(), (uint)intro.length() + 1, this->wrap_column);
    }

    void Parser::help_options(std::ostream &out)
    {
        out << "Options:" << std::endl;
        this->help_options_partial(out, true);
        this->help_options_partial(out, false);
    }

    void Parser::help_options_partial(std::ostream &out, bool named)
    {
        for (const OptionPtr &opt : this->options)
        {
            if (opt->is_named() == named)
            {
                BaseOption::Substitutions subst = {
                    {"%command", platform::path->exec_name()},
                    {"%default", opt->get_default()},
                };

                if (opt->helptext.length() > 0)
                {
                    out << opt->get_description(
                        subst,
                        this->align_column,
                        this->wrap_column);
                }
            }
        }
    }

    void Parser::help_description(std::ostream &out)
    {
        if (this->description.size())
        {
            out << str::wrap(this->description, 0, 0, this->wrap_column, true);
        }
    }

    void Parser::help(const std::string &section,
                      std::ostream &out)
    {
        HelpMethod method;
        try
        {
            method = this->help_sections.at(str::tolower(section));
        }
        catch (const std::out_of_range &)
        {
            throwf(core::exception::InvalidArgument,
                   "No such help section",
                   section);
        }
        method(out);
    }

    void Parser::parse_args(const ArgList &args)
    {
        ParseState state(this->options);
        ArgList::const_iterator it = args.begin();
        ArgList::const_iterator end = args.end();

        while (it != end)
        {
            if (!this->parse_short(&it, end, &state) &&
                !this->parse_long(&it, end, &state) &&
                !this->parse_arg(&it, &state))
            {
                break;
            }
        }
        this->assign_defaults(&state);
    }

    bool Parser::parse_short(ArgList::const_iterator *args_iter,
                             const ArgList::const_iterator &args_end,
                             ParseState *state)
    {
        static const std::regex re("-([^-].*)");
        std::smatch matches;

        // Does args_iter point to a short option string?
        if (std::regex_match(**args_iter, matches, re))
        {
            // Yes, this is a short option string.
            // Walk through each character and find the corresponding option.
            std::string optstring = matches[1];
            std::string::iterator opt_iter = optstring.begin();
            while (opt_iter != optstring.end())
            {
                char optkey = *opt_iter++;
                OptionPtr option = this->get_shortopt(optkey, true);
                state->add_encounter(option, false);

                // We found an option. Does it take an argument?
                if (option->takes_argument())
                {
                    std::string optarg;

                    // Yes. Is the option argument provided in this input
                    // argument (e.g. "-aValue")?
                    if (opt_iter != optstring.end())
                    {
                        // Yes. Extract the argument from the remaining
                        // characters.
                        optarg.assign(opt_iter, optstring.end());
                        opt_iter = optstring.end();
                    }
                    else if ((*args_iter + 1) != args_end)
                    {
                        // No, but we have input argument available.
                        optarg = *++(*args_iter);
                    }
                    else if (option->has_default())
                    {
                        option->assign_default();
                    }
                    else
                    {
                        // No, and there are not more input arguments. This
                        // is an error.
                        throwf(core::exception::InvalidArgument,
                               "Option requires an argument",
                               optkey);
                    }
                    option->assign_argument(optarg);
                }
                else
                {
                    // No argument is required. Parse this option character by itself.
                    option->assign_fixed();
                }
            }

            // We are done with this input arguemnt.
            (*args_iter)++;
            return true;
        }
        else
        {
            // No, there is no short option string here.
            return false;
        }
    }

    bool Parser::parse_long(ArgList::const_iterator *args_iter,
                            const ArgList::const_iterator &args_end,
                            ParseState *state)
    {
        static const std::regex re("--([^=[:space:]]+)(=(.*))?");
        std::smatch matches;

        // Does args_iter point to a long option string?
        if (std::regex_match(*(*args_iter), matches, re))
        {
            // Yes, this is a long option string.
            std::string optkey = matches[1];  // The part before the equal sign, if anything
            std::string optarg = matches[2];  // The part starting with the equal sign
            std::string optval = matches[3];  // The same, sans the equal sign

            OptionPtr option = this->get_longopt(optkey, true);
            state->add_encounter(option, false);

            // Did we capture an equal sign followed by an (possibly-empty) argument?
            bool has_arg = optarg.size();
            // We found an option. Does it take an argument?
            if (option->takes_argument())
            {
                // Yes. Do we need to advance to the next input argument in order
                // to obtain it?
                if (!has_arg)
                {
                    // Yes. See if there are more arguments available.
                    if (++(*args_iter) != args_end)
                    {
                        optval = **args_iter;
                        has_arg = true;
                    }
                }

                if (has_arg)
                {
                    option->assign_argument(optval);
                }
                else if (option->has_default())
                {
                    option->assign_default();
                }
                else if (option->repeats.first > 0)
                {
                    throwf(std::invalid_argument,
                           "Option --%s requires an argument.",
                           optkey);
                }
            }
            else if (optarg.size())
            {
                // No argument is required, but one was provided anyway. Use it.
                option->assign_argument(optval);
            }
            else
            {
                // No argument is required. This means a fixed value exists.
                option->assign_fixed();
            }

            // We are done with this input arguemnt.
            if (*args_iter != args_end)
            {
                (*args_iter)++;
            }
            return true;
        }
        else
        {
            // No, there is no long option string here.
            return false;
        }
    }

    bool Parser::parse_arg(ArgList::const_iterator *args_iter,
                           ParseState *state)
    {
        const OptionPtr &opt = this->next_positional(state);
        opt->assign_argument(**args_iter);
        (*args_iter)++;
        return true;
    }

    OptionPtr Parser::get_shortopt(ShortOpt shortopt, bool required) const
    {
        for (const OptionPtr &opt : this->options)
        {
            for (char candidate : opt->shortopts)
            {
                if (candidate == shortopt)
                {
                    return opt;
                }
            }
        }
        if (required)
        {
            throwf(std::invalid_argument,
                   "Unknown short option \"-%s\"",
                   shortopt);
        }
        return {};
    }

    OptionPtr Parser::get_longopt(const LongOpt &longopt, bool required) const
    {
        for (const OptionPtr &opt : this->options)
        {
            for (const std::string &candidate : opt->longopts)
            {
                if (candidate == longopt)
                {
                    return opt;
                }
            }
        }
        if (required)
        {
            throwf(std::invalid_argument,
                   "Unknown long option \"--%s\"",
                   longopt);
        }
        return {};
    }

    const OptionPtr &Parser::next_positional(ParseState *state) const
    {
        // Is this our first argument?
        if (state->current_unnamed == this->options.end())
        {
            // Yes. Start with the first option.
            state->current_unnamed = this->options.begin();
        }
        // Or, did our previous argument allow for more repeats?
        else if (state->add_encounter(*state->current_unnamed, true))
        {
            // Yes. Return the corresponding option.
            return *state->current_unnamed;
        }
        else if (state->current_unnamed < this->options.end())
        {
            // No. Start from the next argument.
            state->current_unnamed++;
        }

        // Find the next unnamed option
        for (auto it = state->current_unnamed; it != this->options.end(); it++)
        {
            if (!(*it)->is_named() && state->add_encounter(*it, true))
            {
                state->current_unnamed = it;
                return *it;
            }
        }

        // We reached the end of our option list
        throw std::invalid_argument("Too many arguments");
    }

    void Parser::assign_defaults(ParseState *state)
    {
        // Find the next unnamed/positional option
        for (const OptionPtr &opt : this->options)
        {
            uint &encounters = state->encounters[opt.get()];
            uint required = opt->repeats.first;

            if (opt->is_named() && opt->has_default())
            {
                while (encounters < required)
                {
                    opt->assign_default();
                    encounters++;
                }
            }
            else if (encounters < required)
            {
                std::stringstream ss;
                if (opt->is_named())
                {
                    throwf(std::invalid_argument,
                           "Missing option: %s",
                           str::join(opt->optstrings(), "|"));
                }
                else
                {
                    throwf(std::invalid_argument,
                           "Missing argument: %s",
                           opt->argname.value_or("ARG"s));
                }
            }
        }
    }

    //==========================================================================
    // ParseState methods

    Parser::ParseState::ParseState(const OptionList &options)
        : current_unnamed(options.end())
    {
    }

    bool Parser::ParseState::add_encounter(const OptionPtr &opt, bool allowskip)
    {
        auto [minrepeats, maxrepeats] = opt->repeats;

        if (maxrepeats == 0 || this->encounters[opt.get()] < maxrepeats)
        {
            this->encounters[opt.get()]++;
            return true;
        }
        else if (allowskip)
        {
            return false;
        }
        else
        {
            throwf(std::invalid_argument,
                   "Option %s repeated too many times (max allowed is %d)",
                   str::join(opt->optstrings(), "|"),
                   maxrepeats);
        }
    }

}  // namespace core::argparse
