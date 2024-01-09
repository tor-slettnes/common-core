/// -*- c++ -*-
//==============================================================================
/// @file event.h++
/// @brief General base for Event types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "level.h++"
#include "domain.h++"
#include "chrono/date-time.h++"
#include "types/value.h++"
#include "types/streamable.h++"
#include "types/loggable.h++"

#include <string>
#include <ostream>

namespace cc::status
{
    //==========================================================================
    // Field names, e.g. for string representation

    constexpr auto EVENT_FIELD_DOMAIN = "domain";
    constexpr auto EVENT_FIELD_ORIGIN = "origin";
    constexpr auto EVENT_FIELD_LEVEL = "level";
    constexpr auto EVENT_FIELD_TIME = "timepoint";
    constexpr auto EVENT_FIELD_TEXT = "text";
    constexpr auto EVENT_FIELD_ATTRIBUTES = "attributes";

    // using EventCode = std::variant<std::monostate, std::int64_t, std::string>;
    // enum EventCodeSelector = {EVENTCODE_NONE, EVENTCODE_NUMERIC, EVENTCODE_LITERAL};

    //==========================================================================
    // \class Event

    class Event : public types::Streamable,
                  public types::Loggable
    {
    public:
        using Ref = std::shared_ptr<Event>;

    public:
        Event();
        Event(const Event &src);

        Event(const std::string &text,
              Domain domain,
              const std::string &origin,
              Level level = Level::NONE,
              const dt::TimePoint &timepoint = {},
              const types::KeyValueMap &attributes = {});

        virtual Event &operator=(const Event &other) noexcept;
        virtual bool operator==(const Event &other) const noexcept;
        virtual bool operator!=(const Event &other) const noexcept;

    public:
        virtual Domain domain() const noexcept;
        virtual const std::string &origin() const noexcept;
        virtual Level level() const noexcept;
        virtual const dt::TimePoint &timepoint() const noexcept;
        virtual std::string text() const noexcept;
        virtual const types::KeyValueMap &attributes() const noexcept;
        virtual types::KeyValueMap attributes() noexcept;
        virtual types::Value attribute(const std::string &key,
                                       const types::Value &fallback = {}) const noexcept;
        types::TaggedValueList as_tvlist() const noexcept;
        types::KeyValueMap as_kvmap() const noexcept;

    protected:
        virtual std::string class_name() const noexcept
        {
            return "Event";
        }

        virtual void populate_fields(types::TaggedValueList *values) const noexcept;

    public:
        void to_stream(std::ostream &stream) const override;

    protected:
        std::string text_;
        Domain domain_;
        std::string origin_;
        Level level_;
        dt::TimePoint timepoint_;
        types::KeyValueMap attributes_;
    };

    bool operator==(const Event::Ref &lhs, const Event::Ref &rhs);

}  // namespace cc::status
