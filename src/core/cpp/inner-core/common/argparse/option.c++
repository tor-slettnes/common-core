// -*- c++ -*-
//==============================================================================
/// @file option.c++
/// @brief Parse command line arguments, help output
//==============================================================================

#include "option.h++"
#include "string/misc.h++"  // str::join()
#include "status/exceptions.h++"

#include <regex>
#include <sstream>  // std::stringstream

namespace shared::argparse
{
    //==========================================================================
    // BaseOption methods

    BaseOption::BaseOption(const KeyList &keys,
                           const std::optional<std::string> &argname,
                           const std::string &helptext,
                           RepeatSpec repeats)
        : argname(argname),
          helptext(helptext),
          repeats(repeats)
    {
        static const std::regex shortx("-([^-=])");
        static const std::regex longx("--(\\w[\\w-]*)");

        if (keys.size() > 0)
        {
            for (auto &key : keys)
            {
                std::smatch matches;
                if (std::regex_match(key, matches, shortx))
                {
                    this->shortopts.push_back(matches[1].str()[0]);
                }
                else if (std::regex_match(key, matches, longx))
                {
                    this->longopts.push_back(matches[1].str());
                }
                else
                {
                    throwf(shared::exception::InvalidArgument,
                           "Invalid option key",
                           key);
                }
            }
        }
    }

    bool BaseOption::is_named() const
    {
        return this->shortopts.size() + this->longopts.size() > 0;
    }

    std::string BaseOption::get_syntax() const
    {
        std::string optsyntax = str::join(this->optstrings(), "|");
        std::stringstream ss;
        auto [minrepeats, maxrepeats] = this->repeats;

        ss << (minrepeats == 0 ? "[" : "")
           << optsyntax
           << ((optsyntax.length() > 0) && this->argname.has_value() ? "=" : "")
           << this->argname.value_or("")
           << (maxrepeats != 1 ? " ..." : "")
           << (minrepeats == 0 ? "]" : "");
        return ss.str();
    }

    std::string BaseOption::get_description(const Substitutions &subst,
                                            size_t align_column,
                                            size_t wrap_column) const
    {
        std::stringstream ss;
        std::string optsyntax = str::join(this->optstrings(), ", ");

        ss << "    " << optsyntax;
        if (this->argname.has_value())
        {
            ss << (optsyntax.empty() ? "" : "=") << this->argname.value();
        }

        if (!this->helptext.empty())
        {
            size_t current_column = (uint)ss.tellp();
            if (current_column >= align_column)
            {
                ss << std::endl;
                current_column = 0;
            }
            std::string helptext(this->helptext);
            for (const auto &s : subst)
            {
                str::substitute(s.first, s.second, &helptext);
            }

            ss << str::wrap(helptext, current_column, align_column, wrap_column);
        }
        else
        {
            ss << std::endl;
        }
        return ss.str();
    }

    std::vector<std::string> BaseOption::optstrings() const
    {
        std::vector<std::string> opts;
        opts.reserve(this->shortopts.size() + this->longopts.size());
        for (char shortopt : this->shortopts)
        {
            opts.push_back("-"s + shortopt);
        }
        for (std::string longopt : this->longopts)
        {
            opts.push_back("--"s + longopt);
        }
        return opts;
    }

}  // namespace shared::argparse
