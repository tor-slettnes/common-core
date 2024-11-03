/// -*- c++ -*-
//==============================================================================
/// @file event.h++
/// @brief General base for Event types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "domain.h++"
#include "level.h++"
#include "chrono/date-time.h++"
#include "types/value.h++"
#include "types/streamable.h++"
#include "types/partslist.h++"
#include "types/loggable.h++"
#include "types/create-shared.h++"

#include <string>
#include <ostream>

namespace core::status
{
    //==========================================================================
    // Symbols provided here.

    class Event;

    constexpr auto EVENT_FIELD_DOMAIN = "domain";
    constexpr auto EVENT_FIELD_ORIGIN = "origin";
    constexpr auto EVENT_FIELD_CODE = "code";
    constexpr auto EVENT_FIELD_SYMBOL = "symbol";
    constexpr auto EVENT_FIELD_LEVEL = "level";
    constexpr auto EVENT_FIELD_TIME = "timestamp";
    constexpr auto EVENT_FIELD_TEXT = "text";
    constexpr auto EVENT_FIELD_ATTRIBUTES = "attributes";
    constexpr auto EVENT_FIELD_CONTRACT_ID = "contract_id";
    constexpr auto EVENT_FIELD_HOST = "host";

    //==========================================================================
    // \class Event

    class Event : public types::Streamable,
                  public types::Loggable
    {
    public:
        using ptr = std::shared_ptr<Event>;
        using Symbol = std::string;
        using Code = std::int64_t;
        using ContractID = std::string;

        Event();

        Event(Event &&src);

        Event(const Event &src);

        Event(const std::string &text,
              Domain domain,
              const std::string &origin,
              const Code &code,
              const Symbol &symbol,
              Level level = Level::NONE,
              const dt::TimePoint &timepoint = {},
              const types::KeyValueMap &attributes = {},
              const ContractID &contract_id = {},
              const std::string &host = {});

        Event &operator=(Event &&other) noexcept;
        virtual Event &operator=(const Event &other) noexcept;
        virtual bool operator==(const Event &other) const noexcept;
        virtual bool operator!=(const Event &other) const noexcept;

    public:
        virtual Domain domain() const noexcept;
        virtual std::string origin() const noexcept;
        virtual Code code() const noexcept;
        virtual Symbol symbol() const noexcept;
        virtual Level level() const noexcept;
        virtual dt::TimePoint timepoint() const noexcept;
        virtual std::string text() const noexcept;
        virtual const types::KeyValueMap &attributes() const noexcept;
        virtual types::KeyValueMap &attributes() noexcept;
        virtual types::Value attribute(const std::string &key,
                                       const types::Value &fallback = {}) const noexcept;
        virtual ContractID contract_id() const noexcept;
        virtual std::string host() const noexcept;

        types::TaggedValueList as_tvlist() const noexcept;
        types::KeyValueMap as_kvmap() const noexcept;

        virtual bool empty() const noexcept;

    protected:
        virtual std::string class_name() const noexcept;
        virtual void populate_fields(types::PartsList *parts) const noexcept;
        virtual void populate_attributes(types::PartsList *parts) const noexcept;

    public:
        static std::vector<std::string> field_names() noexcept;
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
        dt::TimePoint timepoint_;
        types::KeyValueMap attributes_;
        ContractID contract_id_;
        std::string host_;
    };

    // bool operator==(const Event::ptr &lhs, const Event::ptr &rhs);

}  // namespace core::status
