/// -*- c++ -*-
//==============================================================================
/// @file event.c++
/// @brief General base for Event types: Error, logging::Message, ...
/// @author Tor Slettnes
//==============================================================================

#include "event.h++"

namespace core::status
{
    Event::Event()
        : level_(Level::NONE)
    {
    }

    Event::Event(const Event &src)
    {
        *this = src;
    }

    Event::Event(Event &&src)
    {
        *this = std::move(src);
    }

    Event::Event(const std::string &text,
                 Level level,
                 const std::string &origin,
                 const dt::TimePoint &timepoint,
                 const types::KeyValueMap &attributes)
        : Super(timepoint, attributes),
          text_(text),
          level_(level),
          origin_(origin)
    {
    }

    Event &Event::operator=(Event &&other) noexcept
    {
        Super::operator=(std::move(other));
        std::swap(this->text_, other.text_);
        std::swap(this->level_, other.level_);
        std::swap(this->origin_, other.origin_);
        return *this;
    }

    Event &Event::operator=(const Event &other) noexcept
    {
        Super::operator=(other);
        this->text_ = other.text();
        this->level_ = other.level();
        this->origin_ = other.origin();
        return *this;
    }

    bool Event::operator==(const Event &other) const noexcept
    {
        return Super::operator==(other) &&
               this->equivalent(other);
    }

    bool Event::equivalent(const Event &other) const noexcept
    {
        return (this->text() == other.text()) &&
               (this->level() == other.level()) &&
               (this->origin() == other.origin());
    }

    std::string Event::text() const noexcept
    {
        return this->text_;
    }

    Level Event::level() const noexcept
    {
        return this->level_;
    }

    std::string Event::origin() const noexcept
    {
        return this->origin_;
    }

    std::string Event::class_name() const noexcept
    {
        return "Event";
    }

    std::vector<std::string> Event::event_fields() noexcept
    {
        std::vector<std::string> fields = Loggable::loggable_fields();
        fields.insert(
            fields.end(),
            {
                This::FIELD_TEXT,
                This::FIELD_LEVEL,
                This::FIELD_ORIGIN,
            });
        return fields;
    }

    std::vector<std::string> Event::field_names() const noexcept
    {
        return This::event_fields();
    }

    types::Value Event::get_field_as_value(const std::string &field_name) const
    {
        if (field_name == This::FIELD_TEXT)
        {
            return this->text();
        }
        else if (field_name == This::FIELD_LEVEL)
        {
            return str::convert_from(this->level());
        }
        else if (field_name == This::FIELD_ORIGIN)
        {
            return this->origin();
        }
        else
        {
            return Super::get_field_as_value(field_name);
        }
    }
}  // namespace core::status
