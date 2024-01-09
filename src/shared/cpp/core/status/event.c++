/// -*- c++ -*-
//==============================================================================
/// @file event.c++
/// @brief General base for Event types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "event.h++"
#include "types/create-shared.h++"
#include "string/convert.h++"
#include <algorithm>

namespace cc::status
{
    //==========================================================================
    /// @class Event
    /// @brief General event.

    Event::Event()
        : domain_(Domain::NONE),
          level_(Level::NONE)
    {
    }

    Event::Event(const Event &src)
        : text_(src.text()),
          domain_(src.domain()),
          origin_(src.origin()),
          level_(src.level()),
          timepoint_(src.timepoint()),
          attributes_(src.attributes())
    {
    }

    Event::Event(const std::string &text,
                 Domain domain,
                 const std::string &origin,
                 Level level,
                 const dt::TimePoint &timepoint,
                 const types::KeyValueMap &attributes)
        : text_(text),
          domain_(domain),
          origin_(origin),
          level_(level),
          timepoint_(timepoint),
          attributes_(attributes)
    {
    }

    Event &Event::operator=(const Event &other) noexcept
    {
        this->text_ = other.text();
        this->domain_ = other.domain();
        this->origin_ = other.origin();
        this->level_ = other.level();
        this->timepoint_ = other.timepoint();
        this->attributes_ = other.attributes();
        return *this;
    }

    bool Event::operator==(const Event &other) const noexcept
    {
        return (this->text() == other.text()) &&
               (this->domain() == other.domain()) &&
               (this->origin() == other.origin()) &&
               (this->level() == other.level()) &&
               (this->timepoint() == other.timepoint()) &&
               (this->attributes() == other.attributes());
    }

    bool Event::operator!=(const Event &other) const noexcept
    {
        return !(*this == other);
    }

    Domain Event::domain() const noexcept
    {
        return this->domain_;
    }

    const std::string &Event::origin() const noexcept
    {
        return this->origin_;
    }

    Level Event::level() const noexcept
    {
        return this->level_;
    }

    const dt::TimePoint &Event::timepoint() const noexcept
    {
        return this->timepoint_;
    }

    std::string Event::text() const noexcept
    {
        return this->text_;
    }

    const types::KeyValueMap &Event::attributes() const noexcept
    {
        return this->attributes_;
    }

    types::KeyValueMap Event::attributes() noexcept
    {
        return this->attributes_;
    }

    types::Value Event::attribute(const std::string &key,
                                  const types::Value &fallback) const noexcept
    {
        return this->attributes_.get(key, fallback);
    }

    types::TaggedValueList Event::as_tvlist() const noexcept
    {
        types::TaggedValueList tvlist;
        this->populate_fields(&tvlist);
        return tvlist;
    }

    types::KeyValueMap Event::as_kvmap() const noexcept
    {
        return this->as_tvlist().as_kvmap();
    }

    void Event::populate_fields(types::TaggedValueList *values) const noexcept
    {
        values->reserve(values->size() + 5 + this->attributes_.size());

        if (const std::string &text = this->text(); !text.empty())
        {
            values->emplace_back(EVENT_FIELD_TEXT, text);
        }

        if (Domain domain = this->domain(); domain != Domain::NONE)
        {
            values->emplace_back(EVENT_FIELD_DOMAIN, str::convert_from(domain));
        }

        if (const std::string &origin = this->origin(); !origin.empty())
        {
            values->emplace_back(EVENT_FIELD_ORIGIN, str::convert_from(origin));
        }

        if (Level level = this->level(); level != Level::NONE)
        {
            values->emplace_back(EVENT_FIELD_LEVEL, str::convert_from(level));
        }

        if (const dt::TimePoint &tp = this->timepoint(); tp.time_since_epoch() != dt::Duration::zero())
        {
            values->emplace_back(EVENT_FIELD_TIME, tp);
        }

        values->extend(this->attributes_.as_tvlist());
    }

    void Event::to_stream(std::ostream &stream) const
    {
        types::TaggedValueList fields;
        fields.push_back({{}, this->text()});
        this->populate_fields(&fields);
        fields.filtered_values().to_stream(stream, this->class_name() + "(", ", ", ")");
    }

    bool operator==(const Event::Ref &lhs, const Event::Ref &rhs)
    {
        return (!lhs && !rhs)   ? true
               : (!lhs || !rhs) ? false
               : *lhs == *rhs   ? true
                                : false;
    }

}  // namespace cc::status
