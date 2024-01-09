/// -*- c++ -*-
//==============================================================================
/// @file error.c++
/// @brief General base for Error types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "error.h++"
#include <algorithm>

namespace cc::status
{
    //==========================================================================
    /// @class Event
    /// @brief General error.

    Error::Error()
        : Event(),
          code_(0)
    {
    }

    Error::Error(const std::string &text,
                 Domain domain,
                 const std::string &origin,
                 const Code &code,
                 const Symbol &symbol,
                 Level level,
                 const dt::TimePoint &timepoint,
                 const types::KeyValueMap &attributes)
        : Event(text, domain, origin, level, timepoint, attributes),
          code_(code),
          symbol_(symbol)
    {
    }

    Error::Error(const Error &src)
        : Event(src),
          code_(src.code_),
          symbol_(src.symbol_)
    {
    }

    Error &Error::operator=(const Error &other) noexcept
    {
        Super::operator=(other);
        this->code_ = other.code_;
        this->symbol_ = other.symbol_;
        return *this;
    }

    bool Error::operator==(const Error &other) const noexcept
    {
        return Super::operator==(other) &&
               (this->code() == other.code()) &&
               (this->symbol() == other.symbol());
    }

    bool Error::operator!=(const Error &other) const noexcept
    {
        return !(*this == other);
    }

    bool Error::empty() const noexcept
    {
        return (this->code() == 0) && this->symbol().empty();
    }

    Error::Code Error::code() const noexcept
    {
        return this->code_;
    }

    Error::Symbol Error::symbol() const noexcept
    {
        return this->symbol_;
    }

    std::string Error::class_name() const noexcept
    {
        return "Error";
    }

    void Error::populate_fields(types::TaggedValueList *values) const noexcept
    {
        if (this->code())
        {
            values->emplace_back(ERROR_FIELD_CODE, this->code());
        }

        if (this->symbol().size())
        {
            values->emplace_back(ERROR_FIELD_SYMBOL, this->symbol());
        }

        Super::populate_fields(values);
    }

    bool operator==(const ErrorRef &lhs, const ErrorRef &rhs)
    {
        return (!lhs && !rhs)   ? true
               : (!lhs || !rhs) ? false
               : *lhs == *rhs   ? true
                                : false;
    }
}  // namespace cc::status
