/// -*- c++ -*-
//==============================================================================
/// @file loggable.c++
/// @brief Abstract loggable item, base for telemetry, log entry, error, etc.
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "loggable.h++"

namespace core::types
{
    Loggable::Loggable(const dt::TimePoint &tp,
                       const types::KeyValueMap &attributes)
        : timepoint_(tp),
          attributes_(attributes)
    {
    }

    Loggable::Loggable(Loggable &&src)
    {
        *this = src;
    }

    Loggable::Loggable(const Loggable &src)
    {
        *this = std::move(src);
    }

    Loggable &Loggable::operator=(Loggable &&other) noexcept
    {
        std::swap(this->timepoint_, other.timepoint_);
        std::swap(this->attributes_, other.attributes_);
        return *this;
    }

    Loggable &Loggable::operator=(const Loggable &other) noexcept
    {
        this->timepoint_ = other.timepoint();
        this->attributes_ = other.attributes();
        return *this;
    }

    bool Loggable::operator==(const Loggable &other) const noexcept
    {
        return (this->contract_id() == other.contract_id()) &&
               (this->timepoint() == other.timepoint()) &&
               (this->attributes() == other.attributes());
    }

    bool Loggable::operator!=(const Loggable &other) const noexcept
    {
        return !(*this == other);
    }

    dt::TimePoint Loggable::timepoint() const noexcept
    {
        return this->timepoint_;
    }

    const types::KeyValueMap &Loggable::attributes() const noexcept
    {
        return this->attributes_;
    }

    types::KeyValueMap &Loggable::attributes() noexcept
    {
        return this->attributes_;
    }

    types::Value Loggable::attribute(
        const std::string &key,
        const types::Value &fallback) const noexcept
    {
        return this->attributes_.get(key, fallback);
    }

    std::string Loggable::class_name() const noexcept
    {
        return "Loggable";
    }

    std::vector<std::string> Loggable::loggable_fields() noexcept
    {
        return {
            This::FIELD_TIME,
            This::FIELD_ATTRIBUTES,
        };
    }

    std::vector<std::string> Loggable::field_names() const noexcept
    {
        return This::loggable_fields();
    }

    types::Value Loggable::get_field_as_value(const std::string &field_name) const
    {
        if (field_name == This::FIELD_TIME)
        {
            return this->timepoint();
        }
        else if (field_name == This::FIELD_ATTRIBUTES)
        {
            return this->attributes();
        }
        else
        {
            return this->attribute(field_name);
        }
    }

    void Loggable::to_tvlist(types::TaggedValueList *tvlist) const
    {
        for (const std::string &field_name : this->field_names())
        {
            tvlist->append_if_value(field_name,
                                    this->get_field_as_value(field_name));
        }
    }

    void Loggable::to_stream(std::ostream &stream) const
    {
        stream << this->class_name() << this->as_tvlist();
    }

}  // namespace core::types
