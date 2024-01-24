/// -*- c++ -*-
//==============================================================================
/// @file event.c++
/// @brief General base for Event types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "event.h++"
#include "exceptions.h++"
#include <algorithm>

namespace cc::status
{
    //==========================================================================
    /// @class Event
    /// @brief General event.

    Event::Event()
        : domain_(Domain::NONE),
          level_(Level::NONE),
          flow_(Flow::NONE),
          code_(0)
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
                 Domain domain,
                 const std::string &origin,
                 const Code &code,
                 const Symbol &symbol,
                 Level level,
                 Flow flow,
                 const dt::TimePoint &timepoint,
                 const types::KeyValueMap &attributes)
        : text_(text),
          domain_(domain),
          origin_(origin),
          code_(code),
          symbol_(symbol),
          level_(level),
          flow_(flow),
          timepoint_(timepoint),
          attributes_(attributes)
    {
    }

    Event &Event::operator=(Event &&other) noexcept
    {
        std::swap(this->text_, other.text_);
        std::swap(this->domain_, other.domain_);
        std::swap(this->origin_, other.origin_);
        std::swap(this->code_, other.code_);
        std::swap(this->symbol_, other.symbol_);
        std::swap(this->level_, other.level_);
        std::swap(this->flow_, other.flow_);
        std::swap(this->timepoint_, other.timepoint_);
        std::swap(this->attributes_, other.attributes_);
        return *this;
    }

    Event &Event::operator=(const Event &other) noexcept
    {
        this->text_ = other.text();
        this->domain_ = other.domain();
        this->origin_ = other.origin();
        this->code_ = other.code_;
        this->symbol_ = other.symbol_;
        this->level_ = other.level();
        this->flow_ = other.flow();
        this->timepoint_ = other.timepoint();
        this->attributes_ = other.attributes();
        return *this;
    }

    bool Event::operator==(const Event &other) const noexcept
    {
        return (this->text() == other.text()) &&
               (this->domain() == other.domain()) &&
               (this->origin() == other.origin()) &&
               (this->code() == other.code()) &&
               (this->symbol() == other.symbol()) &&
               (this->level() == other.level()) &&
               (this->flow() == other.flow()) &&
               (this->timepoint() == other.timepoint()) &&
               (this->attributes() == other.attributes());
    }

    bool Event::operator!=(const Event &other) const noexcept
    {
        return !(*this == other);
    }

    bool Event::empty() const noexcept
    {
        return ((this->code() == 0) &&
                this->symbol().empty() &&
                (this->flow() == Flow::NONE));
    }

    Domain Event::domain() const noexcept
    {
        return this->domain_;
    }

    const std::string &Event::origin() const noexcept
    {
        return this->origin_;
    }

    Event::Code Event::code() const noexcept
    {
        return this->code_;
    }

    Event::Symbol Event::symbol() const noexcept
    {
        return this->symbol_;
    }

    Level Event::level() const noexcept
    {
        return this->level_;
    }

    Flow Event::flow() const noexcept
    {
        return this->flow_;
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
        values->reserve(values->size() + 8 + this->attributes_.size());

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

        if (this->code())
        {
            values->emplace_back(EVENT_FIELD_CODE, this->code());
        }

        if (this->symbol().size())
        {
            values->emplace_back(EVENT_FIELD_SYMBOL, this->symbol());
        }

        if (Level level = this->level(); level != Level::NONE)
        {
            values->emplace_back(EVENT_FIELD_LEVEL, str::convert_from(level));
        }

        if (Flow flow = this->flow(); flow != Flow::NONE)
        {
            values->emplace_back(EVENT_FIELD_FLOW, str::convert_from(flow));
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
        // fields.push_back({{}, this->text()});
        this->populate_fields(&fields);
        fields.filtered_values().to_stream(stream, this->class_name() + "(", ", ", ")");
    }

    std::string Event::class_name() const noexcept
    {
        return "Event";
    }

    void Event::throw_if_error() const
    {
        if (std::exception_ptr eptr = this->as_exception_ptr())
        {
            std::rethrow_exception(eptr);
        }
    }

    std::exception_ptr Event::as_exception_ptr() const
    {
        std::exception_ptr eptr = {};
        switch (this->domain())
        {
        case Domain::PERIPHERAL:
            return this->as_device_error();

        case Domain::SYSTEM:
            return this->as_system_error();

        case Domain::APPLICATION:
            return this->as_application_error();

        case Domain::SERVICE:
            return this->as_service_error();

        default:
            return {};
        }
    }

    std::exception_ptr Event::as_device_error() const
    {
        return std::make_exception_ptr<exception::DeviceError>(
            {this->text(),
             this->origin(),
             this->code(),
             this->symbol(),
             this->level(),
             this->flow(),
             this->timepoint(),
             this->attributes()});
    }

    std::exception_ptr Event::as_system_error() const
    {
        types::Value path1 = this->attribute("path1");
        types::Value path2 = this->attribute("path2");
        if (path1 || path2)
        {
            return std::make_exception_ptr<exception::FilesystemError>(*this);
        }
        else
        {
            return std::make_exception_ptr<exception::SystemError>(*this);
        }
    }

    std::exception_ptr Event::as_application_error() const
    {
        return std::make_exception_ptr<exception::UnknownError>(*this);
    }

    std::exception_ptr Event::as_service_error() const
    {
        return std::make_exception_ptr<exception::UnknownError>(*this);
    }

    bool operator==(const Event::Ref &lhs, const Event::Ref &rhs)
    {
        return (!lhs && !rhs)   ? true
               : (!lhs || !rhs) ? false
               : *lhs == *rhs   ? true
                                : false;
    }

}  // namespace cc::status
