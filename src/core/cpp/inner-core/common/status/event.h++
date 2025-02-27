/// -*- c++ -*-
//==============================================================================
/// @file event.h++
/// @brief General base for Event types: Error, logging::Message, ...
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "level.h++"
#include "types/loggable.h++"

namespace core::status
{
    //==========================================================================
    // Symbols provided here.

    class Event;

    //==========================================================================
    // @class Event

    class Event : public types::Loggable
    {
        using This = Event;
        using Super = types::Loggable;

    public:
        using ptr = std::shared_ptr<Event>;

        inline static const std::string FIELD_TEXT = "text";
        inline static const std::string FIELD_LEVEL = "level";
        inline static const std::string FIELD_ORIGIN = "origin";

    public:
        Event();

        Event(Event &&src);

        Event(const Event &src);

        Event(const std::string &text,
              Level level = Level::NONE,
              const std::string &origin = {},
              const dt::TimePoint &timepoint = {},
              const types::KeyValueMap &attributes = {});

        Event &operator=(Event &&other) noexcept;
        Event &operator=(const Event &other) noexcept;
        bool operator==(const Event &other) const noexcept;

    public:
        virtual std::string text() const noexcept;
        virtual Level level() const noexcept;
        virtual std::string origin() const noexcept;

    protected:
        std::string class_name() const noexcept override;

    public:
        static std::vector<std::string> event_fields() noexcept;

    public:
        std::vector<std::string> field_names() const noexcept override;
        types::Value get_field_as_value(const std::string &field_name) const override;

    private:
        std::string text_;
        Level level_ = Level::NONE;
        std::string origin_;
    };
}  // namespace core::status
