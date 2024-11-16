/// -*- c++ -*-
//==============================================================================
/// @file event.c++
/// @brief General base for Event types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "event.h++"
#include "exceptions.h++"
#include <algorithm>

namespace core::status
{
    //==========================================================================
    /// @class Event
    /// @brief General event.

    Event::Event()
        : domain_(Domain::NONE),
          level_(Level::NONE),
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
                 const dt::TimePoint &timepoint,
                 const types::KeyValueMap &attributes,
                 const ContractID &contract_id,
                 const std::string &host)
        : text_(text),
          domain_(domain),
          origin_(origin),
          code_(code),
          symbol_(symbol),
          level_(level),
          timepoint_(timepoint),
          attributes_(attributes),
          contract_id_(contract_id),
          host_(host)
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
        std::swap(this->timepoint_, other.timepoint_);
        std::swap(this->attributes_, other.attributes_);
        std::swap(this->contract_id_, other.contract_id_);
        std::swap(this->host_, other.host_);
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
        this->timepoint_ = other.timepoint();
        this->attributes_ = other.attributes();
        this->contract_id_ = other.contract_id();
        this->host_ = other.host();
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
               (this->timepoint() == other.timepoint()) &&
               (this->attributes() == other.attributes()) &&
               (this->contract_id() == other.contract_id()) &&
               (this->host() == other.host());
    }

    bool Event::operator!=(const Event &other) const noexcept
    {
        return !(*this == other);
    }

    bool Event::empty() const noexcept
    {
        return ((this->code() == 0) &&
                this->symbol().empty() &&
                (this->domain() == Domain::NONE));
    }

    Domain Event::domain() const noexcept
    {
        return this->domain_;
    }

    std::string Event::origin() const noexcept
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

    dt::TimePoint Event::timepoint() const noexcept
    {
        return this->timepoint_;
    }

    std::string Event::text() const noexcept
    {
        return this->text_;
    }

    Event::ContractID Event::contract_id() const noexcept
    {
        return this->contract_id_;
    }

    std::string Event::host() const noexcept
    {
        return this->host_;
    }

    const types::KeyValueMap &Event::attributes() const noexcept
    {
        return this->attributes_;
    }

    types::KeyValueMap &Event::attributes() noexcept
    {
        return this->attributes_;
    }

    types::Value Event::attribute(const std::string &key,
                                  const types::Value &fallback) const noexcept
    {
        return this->attributes_.get(key, fallback);
    }

    void Event::populate_fields(types::TaggedValueList *tvlist) const noexcept
    {
        tvlist->append_if(!this->text().empty(),
                          EVENT_FIELD_TEXT,
                          this->text());

        tvlist->append_if(this->domain() != Domain::NONE,
                          EVENT_FIELD_DOMAIN,
                          str::convert_from(this->domain()));

        tvlist->append_if(!this->origin().empty(),
                          EVENT_FIELD_ORIGIN,
                          this->origin());

        tvlist->append_if(this->code() != 0,
                          EVENT_FIELD_CODE,
                          this->code());

        tvlist->append_if(!this->symbol().empty(),
                          EVENT_FIELD_SYMBOL,
                          this->symbol());

        tvlist->append_if(this->level() != Level::NONE,
                          EVENT_FIELD_LEVEL,
                          str::convert_from(this->level()));

        tvlist->append_if(this->timepoint() != dt::epoch,
                          EVENT_FIELD_TIME,
                          this->timepoint());

        tvlist->append_if(!this->contract_id().empty(),
                          EVENT_FIELD_CONTRACT_ID,
                          this->contract_id());

        tvlist->append_if(!this->host().empty(),
                          EVENT_FIELD_HOST,
                          this->host());
    }

    void Event::populate_attributes(types::TaggedValueList *tvlist) const noexcept
    {
        tvlist->insert(tvlist->end(),
                       this->attributes().begin(),
                       this->attributes().end());
    }

    std::vector<std::string> Event::field_names() noexcept
    {
        return {
            EVENT_FIELD_TEXT,
            EVENT_FIELD_TIME,
            EVENT_FIELD_LEVEL,
            EVENT_FIELD_DOMAIN,
            EVENT_FIELD_ORIGIN,
            EVENT_FIELD_CODE,
            EVENT_FIELD_SYMBOL,
            EVENT_FIELD_HOST,
            EVENT_FIELD_CONTRACT_ID,
        };
    }

    void Event::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        this->populate_fields(tvlist);
        this->populate_attributes(tvlist);
    }

    void Event::to_stream(std::ostream &stream) const
    {
        stream << this->class_name() << this->as_tvlist();
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
        case Domain::DEVICE:
            return this->as_device_error();

        case Domain::SYSTEM:
            return this->as_system_error();

        case Domain::APPLICATION:
            return this->as_application_error();

        case Domain::SERVICE:
            return this->as_service_error();

        default:
            if (this->code() || !this->symbol().empty())
            {
                return this->as_application_error();
            }
            else
            {
                return {};
            }
        }
    }

    std::exception_ptr Event::as_device_error() const
    {
        return std::make_exception_ptr<exception::DeviceError>(
            Event(*this));
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

    // bool operator==(const Event::ptr &lhs, const Event::ptr &rhs)
    // {
    //     return (!lhs && !rhs)   ? true
    //            : (!lhs || !rhs) ? false
    //            : *lhs == *rhs   ? true
    //                             : false;
    // }

}  // namespace core::status
