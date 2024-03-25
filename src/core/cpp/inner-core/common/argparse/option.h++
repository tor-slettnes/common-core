// -*- c++ -*-
//==============================================================================
/// @file option.h++
/// @brief Parse command line arguments, help output
//==============================================================================

#pragma once
#include "string/convert.h++"

#include <functional>  // std::function<T>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <list>
#include <variant>

/// Argument parser utility, loosely modeled after Python's "argparse" module.
namespace core::argparse
{
    /// A (min,max) pair indicating number of times the option must/can be repeated.
    /// Zero means no lower/upper limit.
    using RepeatSpec = std::pair<uint, uint>;

    /// Options
    using ShortOpt = char;
    using LongOpt = std::string;

    /// A list of argument strings
    using ArgList = std::vector<std::string>;

    /// A list of option names
    using KeyList = std::vector<LongOpt>;

    /// Abstract base for Option<T> template, below.
    class BaseOption
    {
    public:
        using Substitutions = std::unordered_map<std::string, std::string>;

        /// Constructor, inherited by Option<T> and Argument<T>, below.
        BaseOption(const KeyList &keys,
                   const std::optional<std::string> &argname,
                   const std::string &helptext,
                   RepeatSpec repeats);

        virtual bool takes_argument() const = 0;
        virtual bool has_default() const = 0;
        virtual std::string get_default() const = 0;
        virtual void assign_argument(const std::string &arg) const = 0;
        virtual void assign_fixed() const = 0;
        virtual void assign_default() const = 0;

        bool is_named() const;
        std::string get_syntax() const;
        std::string get_description(const Substitutions &subst,
                                    size_t align_column,
                                    size_t wrap_column) const;
        std::vector<std::string> optstrings() const;

    public:
        std::optional<std::string> argname;
        std::vector<char> shortopts;
        std::vector<std::string> longopts;
        std::string helptext;
        RepeatSpec repeats;
    };

    /// A list of pointers to Option<T> (BaseOption derivative) instances.
    using OptionPtr = std::shared_ptr<const BaseOption>;
    using OptionList = std::vector<OptionPtr>;

    template <class T = std::string>
    using Target = std::variant<
        T *,                     // Pointer to storage location
        std::vector<T> *,        // Pointer to list
        std::function<void(T)>,  // Callback with argument
        std::function<void()>>;  // Callback without argument

    /// Option template, with data type as template argument.
    ///
    /// Multiple constructors are provided, depending on whether the result is to be stored in
    ///     an atomic variable, appended to a vector, or passed via a callback function.
    ///
    /// \param[in] keys
    ///     Zero or more option keys, each conforming to one of the following:
    ///       - Short format, comprising a single dash followed by a single
    ///         letter (e.g. "-s").
    ///       - Long format, comprising two dashes followed by one or more
    ///         hyphen-delimited words ("--some-long-option").
    ///     With no option keys, this becomes a positional argument.
    ///
    /// \param[in] argname
    ///     Argument name. If provided, the option will take an argument. This
    ///     is required for options with no keys, i.e. positional arguments.
    ///
    /// \param[in] helptext
    ///     Human readable text explaining this option. The following
    ///     substitutions take place within this text:
    ///        %command is replaced with the command name (e.g., argv[0])
    ///        %default is replaced with the default value, if any.
    ///
    /// \param[in] target
    ///     Pointer to a variable in which the parsed value will be stored.
    ///
    /// \param[in] targetVector
    ///     Pointer to a vector to which the parsed value will be appended.
    ///
    /// \param[in] callback
    ///     Pointer to function that will receive the parsed value.
    ///
    /// \param[in] constValue
    ///     Optional fixed value to store in \p *target if this option is
    ///     found during parsing. If not provided here, an option argument
    ///     is expected.
    ///
    /// \param[in] defaultValue
    ///     Optional value to store in \p *target if this option is absent
    ///     among input arguments. If not provided, such an absence will
    ///     leave \p *target untouched.
    ///
    /// \param[in] repeats
    ///     Pair of (min, max) values indicating how many times this
    ///     option is required/allowed.

    template <class T>
    class Option : public BaseOption
    {
    public:
        /// Option that stores result in an atomic variable
        Option(const KeyList &keys,
               const std::optional<std::string> &argname,
               const std::string &helptext,
               const Target<T> &target,
               const std::optional<T> &constValue,
               const std::optional<T> &defaultValue,
               RepeatSpec repeats)
            : BaseOption(keys, argname, helptext, repeats),
              target(target),
              constValue(constValue),
              defaultValue(defaultValue)  // ,
                                          // typeconvert(str::convert_to<T>)
        {
        }

    public:
        // Determine whether this option expects an argument.
        bool takes_argument() const override
        {
            return (this->argname.has_value());
        }

        // Determine whether this option has a default value.
        bool has_default() const override
        {
            return this->defaultValue.has_value();
        }

        // Determine whether this option has a default value.
        std::string get_default() const override
        {
            if (this->defaultValue.has_value())
            {
                return str::convert_from(this->defaultValue.value());
            }
            else if (auto *target = std::get_if<T *>(&this->target))
            {
                return str::convert_from(**target);
            }
            else
            {
                return "(no default)";
            }
        }

        // Assign option argument from an input string
        inline void assign_argument(const std::string &arg) const override
        {
            this->assign_value(str::convert_to<T>(arg));
        }

        // Assign a fixed for an option that does not take an argument.
        inline void assign_fixed() const override
        {
            if (this->constValue.has_value())
            {
                this->assign_value(this->constValue.value());
            }
        }

        // Assign the default value for an option that was not invoked.
        inline void assign_default() const override
        {
            if (this->defaultValue.has_value())
            {
                this->assign_value(this->defaultValue.value());
            }
        }

    private:
        virtual inline void assign_value(const T &value) const
        {
            if (auto *target = std::get_if<T *>(&this->target))
            {
                **target = value;
            }
            else if (auto *target = std::get_if<std::vector<T> *>(&this->target))
            {
                (*target)->emplace_back(value);
            }
            else if (auto *target = std::get_if<std::function<void(T)>>(&this->target))
            {
                (*target)(value);
            }
            else if (auto *target = std::get_if<std::function<void()>>(&this->target))
            {
                (*target)();
            }
        }

    protected:
        Target<T> target;
        std::optional<T> constValue;
        std::optional<T> defaultValue;
        // std::function<T(const std::string &s, std::exception_ptr *eptr)> typeconvert;
    };

}  // namespace core::argparse
