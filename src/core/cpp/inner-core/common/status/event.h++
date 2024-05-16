/// -*- c++ -*-
//==============================================================================
/// @file event.h++
/// @brief General base for Event types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "domain.h++"
#include "level.h++"
#include "flow.h++"
#include "chrono/date-time.h++"
#include "types/value.h++"
#include "types/streamable.h++"
#include "types/loggable.h++"
#include "types/create-shared.h++"

#include <string>
#include <ostream>

namespace cc::status
{
    //==========================================================================
    // Symbols provided here.

    class Event;

    //==========================================================================
    // Field names, e.g. for string representation

    constexpr auto EVENT_FIELD_DOMAIN = "domain";
    constexpr auto EVENT_FIELD_ORIGIN = "origin";
    constexpr auto EVENT_FIELD_CODE = "code";
    constexpr auto EVENT_FIELD_SYMBOL = "symbol";
    constexpr auto EVENT_FIELD_LEVEL = "level";
    constexpr auto EVENT_FIELD_FLOW = "flow";
    constexpr auto EVENT_FIELD_TIME = "timepoint";
    constexpr auto EVENT_FIELD_TEXT = "text";
    constexpr auto EVENT_FIELD_ATTRIBUTES = "attributes";

    //==========================================================================
    // \class Event

    class Event : public types::Streamable,
                  public types::Loggable
    {
    public:
        using Ref = std::shared_ptr<Event>;
        using Symbol = std::string;
        using Code = std::int64_t;

        Event();

        Event(Event &&src);

        Event(const Event &src);

        Event(const std::string &text,
              Domain domain,
              const std::string &origin,
              const Code &code,
              const Symbol &symbol,
              Level level = Level::NONE,
              Flow flow = Flow::NONE,
              const dt::TimePoint &timepoint = {},
              const types::KeyValueMap &attributes = {});

        Event &operator=(Event &&other) noexcept;
        virtual Event &operator=(const Event &other) noexcept;
        virtual bool operator==(const Event &other) const noexcept;
        virtual bool operator!=(const Event &other) const noexcept;

    public:
        virtual Domain domain() const noexcept;
        virtual const std::string &origin() const noexcept;
        virtual Code code() const noexcept;
        virtual Symbol symbol() const noexcept;
        virtual Level level() const noexcept;
        virtual Flow flow() const noexcept;
        virtual const dt::TimePoint &timepoint() const noexcept;
        virtual std::string text() const noexcept;
        virtual const types::KeyValueMap &attributes() const noexcept;
        virtual types::KeyValueMap &attributes() noexcept;
        virtual types::Value attribute(const std::string &key,
                                       const types::Value &fallback = {}) const noexcept;
        types::TaggedValueList as_tvlist() const noexcept;
        types::KeyValueMap as_kvmap() const noexcept;

        virtual bool empty() const noexcept;

    protected:
        virtual std::string class_name() const noexcept;
        virtual void populate_fields(types::TaggedValueList *values) const noexcept;

    public:
        virtual void throw_if_error() const;
        virtual std::exception_ptr as_exception_ptr() const;

    protected:
        virtual std::exception_ptr as_device_error() const;
        virtual std::exception_ptr as_system_error() const;
        virtual std::exception_ptr as_application_error() const;
        virtual std::exception_ptr as_service_error() const;

    public:
        void to_stream(std::ostream &stream) const override;

    protected:
        std::string text_;
        Domain domain_;
        std::string origin_;
        Code code_;
        Symbol symbol_;
        Level level_;
        Flow flow_;
        dt::TimePoint timepoint_;
        types::KeyValueMap attributes_;
    };

    bool operator==(const Event::Ref &lhs, const Event::Ref &rhs);

}  // namespace cc::status
