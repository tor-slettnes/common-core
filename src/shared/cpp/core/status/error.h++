/// -*- c++ -*-
//==============================================================================
/// @file error.h++
/// @brief General base for Error types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "event.h++"
#include "types/create-shared.h++"

namespace cc::status
{
    //==========================================================================
    // Symbols provided here.

    class Error;
    using ErrorRef = std::shared_ptr<Error>;

    //==========================================================================
    // Field names, e.g. for string representation

    constexpr auto ERROR_FIELD_CODE = "code";
    constexpr auto ERROR_FIELD_SYMBOL = "symbol";

    //==========================================================================
    // \class Error

    class Error : public Event
    {
        using Super = Event;

    public:
        using Symbol = std::string;
        using Code = std::int64_t;

        // Inherit constructors
        using Event::Event;

        Error();

        Error(const std::string &text,
              Domain domain,
              const std::string &origin,
              const Code &code,
              const Symbol &symbol,
              Level level = Level::NONE,
              const dt::TimePoint &timepoint = {},
              const types::KeyValueMap &attributes = {});

        Error(const Event &src,
              const Code &code = 0,
              const Symbol &symbol = {});

        Error(const Error &src);

        virtual Error &operator=(const Error &other) noexcept;
        virtual bool operator==(const Error &other) const noexcept;
        virtual bool operator!=(const Error &other) const noexcept;

    public:
        virtual bool empty() const noexcept;
        virtual Code code() const noexcept;
        virtual Symbol symbol() const noexcept;

    protected:
        std::string class_name() const noexcept override;
        void populate_fields(types::TaggedValueList *values) const noexcept override;

    protected:
        Code code_;
        Symbol symbol_;
    };

    bool operator==(const ErrorRef &lhs, const ErrorRef &rhs);

}  // namespace cc::status
