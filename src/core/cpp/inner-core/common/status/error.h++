/// -*- c++ -*-
//==============================================================================
/// @file error.h++
/// @brief General base for Error types
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "event.h++"
#include "domain.h++"

namespace core::status
{
    //==========================================================================
    // Symbols provided here.

    class Error;

    //==========================================================================
    // @class Error

    class Error : public Event
    {
        using This = Error;
        using Super = Event;

    public:
        using ptr = std::shared_ptr<Error>;
        using Symbol = std::string;
        using Code = std::int64_t;

        inline static const std::string FIELD_DOMAIN = "domain";
        inline static const std::string FIELD_CODE = "code";
        inline static const std::string FIELD_SYMBOL = "symbol";
        inline static const std::string CONTRACT = "error";

    public:
        Error();

        Error(Error &&src);

        Error(const Error &src);

        Error(const std::string &text,
              Domain domain,
              const std::string &origin,
              const Code &code,
              const Symbol &symbol,
              Level level = Level::NONE,
              const dt::TimePoint &timepoint = {},
              const types::KeyValueMap &attributes = {});

        Error &operator=(Error &&other) noexcept;
        Error &operator=(const Error &other) noexcept;
        bool equivalent(const Event &other) const noexcept override;

        /// @brief
        ///     Determine if this Status object is fundamentally equivalent to another.
        /// @param[in]
        ///     Other event/Status object with which this is to be compared
        /// @description
        ///     Equivalence is based on object attributes that are deemed
        ///     sufficient to distinghish a repeated instance of this from
        ///     another type of status.  For example, status codes and texts are
        ///     compared, but not timestamps.
        std::string contract_id() const noexcept override;
        virtual Domain domain() const noexcept;
        virtual Code code() const noexcept;
        virtual Symbol symbol() const noexcept;
        virtual bool empty() const noexcept;

        void to_stream(std::ostream &stream) const override;
        void to_literal_stream(std::ostream &stream) const override;

    protected:
        std::string class_name() const noexcept override;

    public:
        static std::vector<std::string> error_fields() noexcept;
        std::vector<std::string> field_names() const noexcept override;
        types::Value get_field_as_value(const std::string &field_name) const override;

    public:
        virtual void throw_if_error() const;
        virtual std::exception_ptr as_exception_ptr() const;

    protected:
        virtual std::exception_ptr as_device_error() const;
        virtual std::exception_ptr as_system_error() const;
        virtual std::exception_ptr as_application_error() const;
        virtual std::exception_ptr as_service_error() const;

    private:
        Domain domain_ = Domain::NONE;
        Code code_ = 0;
        Symbol symbol_;
    };

}  // namespace core::status
