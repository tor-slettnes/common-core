/// -*- c++ -*-
//==============================================================================
/// @file error.c++
/// @brief General base for Error types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "event.h++"
#include "exceptions.h++"
#include <algorithm>

namespace core::status
{
    //==========================================================================
    /// @class Error
    /// @brief General event.

    Error::Error()
        : domain_(Domain::NONE),
          code_(0)
    {
    }

    Error::Error(const Error &src)
    {
        *this = src;
    }

    Error::Error(Error &&src)
    {
        *this = std::move(src);
    }

    Error::Error(const std::string &text,
                 Domain domain,
                 const std::string &origin,
                 const Code &code,
                 const Symbol &symbol,
                 Level level,
                 const dt::TimePoint &timepoint,
                 const types::KeyValueMap &attributes)
        : Super(text, level, origin, timepoint, attributes),
          domain_(domain),
          code_(code),
          symbol_(symbol)
    {
    }

    Error &Error::operator=(Error &&other) noexcept
    {
        Super::operator=(std::move(other));
        std::swap(this->domain_, other.domain_);
        std::swap(this->code_, other.code_);
        std::swap(this->symbol_, other.symbol_);
        return *this;
    }

    Error &Error::operator=(const Error &other) noexcept
    {
        Super::operator=(other);
        this->domain_ = other.domain();
        this->code_ = other.code();
        this->symbol_ = other.symbol();
        return *this;
    }

    bool Error::equivalent(const Event &other) const noexcept
    {
        if (auto *that = dynamic_cast<const Error *>(&other))
        {
            return Super::equivalent(other) &&
                   (this->domain() == that->domain()) &&
                   (this->code() == that->code()) &&
                   (this->symbol() == that->symbol()) &&
                   (this->contract_id() == that->contract_id());
        }
        else
        {
            return false;
        }
    }

    std::string Error::contract_id() const noexcept
    {
        return This::CONTRACT;
    }

    Domain Error::domain() const noexcept
    {
        return this->domain_;
    }

    Error::Code Error::code() const noexcept
    {
        return this->code_;
    }

    Error::Symbol Error::symbol() const noexcept
    {
        return this->symbol_;
    }

    bool Error::empty() const noexcept
    {
        return ((this->code() == 0) &&
                this->symbol().empty() &&
                (this->domain() == Domain::NONE));
    }

    std::string Error::class_name() const noexcept
    {
        return "Error";
    }

    void Error::to_stream(std::ostream &stream) const
    {
        stream << "["
               << this->class_name()
               << "] "
               << this->text();

        if (!this->attributes().empty())
        {
            stream << ", attributes="
                   << this->attributes();
        }
    }

    void Error::to_literal_stream(std::ostream &stream) const
    {
        return Super::to_stream(stream);
    }

    std::vector<std::string> Error::error_fields() noexcept
    {
        std::vector<std::string> fields = Event::event_fields();
        fields.insert(
            fields.end(),
            {
                This::FIELD_DOMAIN,
                This::FIELD_CODE,
                This::FIELD_SYMBOL,
            });
        return fields;
    }

    std::vector<std::string> Error::field_names() const noexcept
    {
        return This::error_fields();
    }

    types::Value Error::get_field_as_value(const std::string &field_name) const
    {
        if (field_name == This::FIELD_DOMAIN)
        {
            return str::convert_from(this->domain());
        }
        else if (field_name == This::FIELD_CODE)
        {
            return this->code();
        }
        else if (field_name == This::FIELD_SYMBOL)
        {
            return this->symbol();
        }
        else
        {
            return Super::get_field_as_value(field_name);
        }
    }

    void Error::throw_if_error() const
    {
        if (std::exception_ptr eptr = this->as_exception_ptr())
        {
            std::rethrow_exception(eptr);
        }
    }

    std::exception_ptr Error::as_exception_ptr() const
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

    std::exception_ptr Error::as_device_error() const
    {
        return std::make_exception_ptr<exception::DeviceError>(*this);
    }

    std::exception_ptr Error::as_system_error() const
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

    std::exception_ptr Error::as_application_error() const
    {
        return std::make_exception_ptr<exception::UnknownError>(*this);
    }

    std::exception_ptr Error::as_service_error() const
    {
        return std::make_exception_ptr<exception::UnknownError>(*this);
    }

}  // namespace core::status
