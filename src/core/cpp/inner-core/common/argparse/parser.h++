// -*- c++ -*-
//==============================================================================
/// @file parser.h++
/// @brief Parse command line arguments, help output
//==============================================================================

#pragma once
#include "option.h++"
#include "string/convert.h++"

#include <cassert>
#include <functional>  // std::function<T>
#include <iostream>    // std::cout
#include <optional>
#include <string>
#include <map>

/// Argument parser utility, loosely modeled after Python's "argparse" module.
namespace core::argparse
{
    //==========================================================================
    /// \brief Argument parser, loosely modeled after Python's argparse module.
    ///
    /// See source/helloworld/cpp/server/options.shared
    ///
    class Parser
    {
        class ParseState
        {
        public:
            ParseState(const OptionList &options);
            bool add_encounter(const OptionPtr &option, bool allowskip);

        public:
            std::unordered_map<const BaseOption *, uint> encounters;
            OptionList::const_iterator current_unnamed;
        };

        using HelpMethod = std::function<void(std::ostream &)>;
        using SectionMap = std::map<std::string, HelpMethod>;

    public:
        const RepeatSpec AtMostOne = {0, 1};
        const RepeatSpec ExactlyOne = {1, 1};
        const RepeatSpec OneOrMore = {1, 0};

    public:
        /// \brief Constructor.
        /// \param[in] align_column
        ///     Left-align overflowing help texts at this column
        /// \param[in] wrap_column
        ///     Wrap overflowing help texts before this column

        Parser(size_t align_column = 24,
               size_t wrap_column = 80);

        /// \brief Add description to explain the program's function
        /// \param[in] description
        ///     Optional text explaining the program's function.

        void describe(const std::string &description);

        /// \brief Add an already-constructed option.
        /// \param[in] option
        ///     Option instance
        ///
        /// \note The option is referenced via pointer, so must remain in scope during parsing.
        void add(const OptionPtr &option);

    private:
        /// Instantiate a new option, keeping track on it for subsequent deletion
        template <class Type, class Storage = Target<Type>>
        inline void add(const KeyList &keys,
                        const std::optional<std::string> &argname,
                        const std::string &helptext,
                        Storage target,
                        const std::optional<Type> &constValue,
                        const std::optional<Type> &defaultValue,
                        RepeatSpec repeats)
        {
            this->options.push_back(std::make_shared<Option<Type>>(
                keys, argname, helptext, target, constValue, defaultValue, repeats));
        }

    public:
        /// Simple callback without arguments if encountered
        inline void add_void(const KeyList &keys,
                             const std::string &helptext,
                             std::function<void()> callback)
        {
            this->add<bool, std::function<void()>>(
                keys, {}, helptext, callback, true, {}, AtMostOne);
        }

        /// Simple boolean switch
        template <class Storage = Target<bool>>
        inline void add_flag(const KeyList &keys,
                             const std::string &helptext,
                             Storage target,
                             bool defaultValue = false)
        {
            this->add<bool, Storage>(
                keys, {}, helptext, target, true, defaultValue, ExactlyOne);
        }

        /// Named option with constant value if encountered, default value otherwise
        template <class Type, class Storage = Target<Type>>
        inline void add_const(const KeyList &keys,
                              const std::string &helptext,
                              Storage target,
                              const Type &constValue,
                              const std::optional<Type> &defaultValue = {})
        {
            this->add<Type, Storage>(
                keys, {}, helptext, target, constValue, defaultValue, defaultValue ? ExactlyOne : AtMostOne);
        }

        /// Named option with argument, default value if not encountered
        template <class Type = std::string, class Storage = Target<Type>>
        inline void add_opt(const KeyList &keys,
                            const std::string &argname,
                            const std::string &helptext,
                            Storage target,
                            const Type &defaultValue,
                            bool required = true)
        {
            this->add<Type, Storage>(
                keys, argname, helptext, target, {}, defaultValue, required ? ExactlyOne : AtMostOne);
        }

        /// Named option with argument, default value if not encountered
        template <class Type = std::string, class Storage = Target<Type>>
        inline void add_opt(const KeyList &keys,
                            const std::string &argname,
                            const std::string &helptext,
                            Storage target,
                            const std::optional<Type> &defaultValue = {})
        {
            this->add<Type, Storage>(
                keys, argname, helptext, target, {}, defaultValue, defaultValue ? ExactlyOne : AtMostOne);
        }

        /// Named argument, no default
        template <class Type = std::string, class Storage = Target<Type>>
        inline void add_arg(const KeyList &keys,
                            const std::string &argname,
                            const std::string &helptext,
                            Storage target)
        {
            this->add<Type, Storage>(
                keys, argname, helptext, target, {}, {}, ExactlyOne);
        }

        /// Unnamed argument, with default value if not encountered
        template <class Type = std::string, class Storage = Target<Type>>
        inline void add_arg(const std::string &argname,
                            const std::string &helptext,
                            Storage target,
                            const Type &defaultValue)
        {
            this->add<Type, Storage>(
                {}, argname, helptext, target, {}, defaultValue, AtMostOne);
        }

        /// Unnamed argument, no default
        template <class Type = std::string, class Storage = Target<Type>>
        inline void add_arg(const std::string &argname,
                            const std::string &helptext,
                            Storage target,
                            RepeatSpec repeats = {1, 1})
        {
            this->add<Type, Storage>(
                {}, argname, helptext, target, {}, {}, repeats);
        }

        void add_help_section(const std::string &section,
                              const std::function<void(std::ostream &out)> &method);

        virtual void help_all(std::ostream &out);
        void help_section_list(std::ostream &out);
        void help_usage(std::ostream &out);
        void help_options(std::ostream &out);
        void help_options_partial(std::ostream &out, bool named);
        virtual void help_description(std::ostream &out);

        /// Print usage
        virtual void help(const std::string &section = "all",
                          std::ostream &out = std::cout);

        /// \brief Parse a vector of strings
        /// \param args
        ///     Input arguments to parse
        /// \exception exception::NotFound
        ///     Unknown option name, required option/argument not found
        /// \exception exception::InvalidArgument
        ///     Missing argument
        /// \exception exception::Extraneous
        ///     Too many arguments
        /// \exception std::invalid_argument
        ///     Argument could not be converted to the desired type
        /// \exception std::out_of_range
        ///     Argument is not within supported range for the desired value type
        void parse_args(const ArgList &args);

    private:
        /// \fn parse_short
        ///
        /// \brief
        ///     Parse a short option string starting with a single dash
        ///     (\e -O[...][ARGUMENT]).
        ///
        /// \param[in,out] args_iter
        ///     Iterator to next argument to be parsed. If this is a short
        ///     option string, it is incremented once or twice (depending on
        ///     whether the an additional option argument was consumed).
        ///
        /// \param[in] args_end
        ///     Iterator to the end of the argument list, needed to check bounds.
        ///
        /// \param[in,out] state
        ///     Internal state to track progress
        ///
        /// \return
        ///     Whether or not a short argument string was parsed.
        ///
        /// \exception
        ///     exception::InvalidArgument Invalid short option
        ///     std::invalid_argument Argument could not be converted to the desired type
        ///     std::out_of_range Argument is out of range for the desired type
        ///
        /// If \p args_iter points to a string that starts with a single dash,
        /// process it as a short option string. Iterate over the following
        /// characters and process each corresponding option in turn, until (a)
        /// an option that takes an argument is encountered, or (b) the end of
        /// the string. In the former case, the remainder of the string is
        /// consumed as that argument. In the latter case, if the last option
        /// requires an argument, the following input argument is consumed.
        /// In either case, the function returns true.
        ///
        /// If \p args_iter does not point to a short option string, it is
        /// left intact, and this function returns false.

        bool parse_short(ArgList::const_iterator *args_iter,
                         const ArgList::const_iterator &args_end,
                         ParseState *state);

        /// \fn parse_long
        /// \brief
        ///     Parse a long option string starting with two dashes
        ///     (\e --OPTION[=ARGUMENT]).
        ///
        /// \param[in,out] args_iter
        ///     Iterator to next argument to be parsed. If this is a long
        ///     option string, it is incremented once or twice (depending on
        ///     whether the an additional option argument was consumed).
        ///
        /// \param[in] args_end
        ///     Iterator to the end of the argument list, needed to check bounds.
        ///
        /// \param[in,out] state
        ///     Internal state to track progress
        ///
        /// \return
        ///     Whether or not a long argument string was parsed.
        ///
        /// \exception
        ///     exception::InvalidArgument Invalid short option
        ///     std::invalid_argument Argument could not be converted to the desired type
        ///     std::out_of_range Argument is out of range for the desired type
        ///
        /// If \p args_iter points a string that starts with two dashes, process
        /// it as a long option string. The option name follows the two dashes
        /// up to either (a) an equal sign, or (b) the end of the string is
        /// encountered. In the former case, the portion following the equal
        /// sign is the option argument; in the latter, if the corresponding
        /// option requires an argument, \p args_iter is advanced and the
        /// next input argument is consumed for this purpose. In either case,
        /// the function returns true.
        ///
        /// If \p args_iter does not point to a short option string, it is
        /// left intact, and the function returns false.
        ///
        bool parse_long(ArgList::const_iterator *args_iter,
                        const ArgList::const_iterator &args_end,
                        ParseState *state);

        /// \fn parse_arg
        ///
        /// \brief
        ///     Parse a unnamed (positional) argument string.
        ///
        /// \param[in,out] args_iter
        ///     Iterator to next argument to be parsed. If this is a long
        ///     option string, it is incremented once or twice (depending on
        ///     whether the an additional option argument was consumed).
        ///
        /// \param[in] args_end
        ///     Iterator to the end of the argument list, needed to check bounds.
        ///
        /// \param[in,out] state
        ///     Internal state to track progress
        ///
        /// \return
        ///     Always true (for consistency with parse_short() and parse_long()).
        ///
        /// \exception
        ///     exception::InvalidArgument Invalid short option
        ///     std::invalid_argument Argument could not be converted to the desired type
        ///     std::out_of_range Argument is out of range for the desired type
        ///
        /// Process the next input argument as an unnamed (a.k.a. positional)
        /// argument, then advance \p args_iter to the next input argument.

        bool parse_arg(ArgList::const_iterator *args_iter,
                       ParseState *state);

        /// Fill in default values for missing arguments.
        void assign_defaults(ParseState *state);

        /// Look up Option by short option key
        OptionPtr get_shortopt(ShortOpt shortopt, bool required = false) const;

        /// Look up Option by long option name
        OptionPtr get_longopt(const LongOpt &longopt, bool required = false) const;

        /// Look up the next argument
        const OptionPtr &next_positional(ParseState *state) const;

    public:
        SectionMap help_sections;
        size_t align_column;
        size_t wrap_column;
        OptionList options;
        std::string description;
    };
}  // namespace core::argparse
