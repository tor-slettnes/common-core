/// -*- c++ -*-
//==============================================================================
/// @file loggable.h++
/// @brief Abstract loggable item, base for telemetry, log entry, error, etc.
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "listable.h++"
#include "chrono/date-time.h++"
#include "types/value.h++"

#include <memory>

namespace core::types
{
    //==========================================================================
    /// @class Loggable
    /// @brief Abstract loggable item.
    ///
    /// Specializations include:
    ///   - `status::Event`, and in turn `status::Error` and `logging:Message`
    ///   - `status::Telemetry`
    ///   - `types::BinaryData`


    class Loggable : public Listable
    {
        using This = Loggable;

    public:
        using ContractID = std::string;
        using ptr = std::shared_ptr<Loggable>;

        inline static const std::string FIELD_TIME = "timestamp";
        inline static const std::string FIELD_ATTRIBUTES = "attributes";

    protected:
        Loggable(const dt::TimePoint &tp = {},                // origin timestmap
                 const types::KeyValueMap &attributes = {});  // loggable data

        Loggable(Loggable &&src);
        Loggable(const Loggable &src);

        virtual ~Loggable() {}  // Hack to ensure class is polymorphic

        Loggable &operator=(Loggable &&other) noexcept;
        Loggable &operator=(const Loggable &other) noexcept;
        bool operator==(const Loggable &other) const noexcept;
        bool operator!=(const Loggable &other) const noexcept;

    public:
        virtual ContractID contract_id() const noexcept = 0;
        virtual dt::TimePoint timepoint() const noexcept;
        virtual const types::KeyValueMap &attributes() const noexcept;
        virtual types::KeyValueMap &attributes() noexcept;
        virtual types::Value attribute(
            const std::string &key,
            const types::Value &fallback = {}) const noexcept;

    protected:
        virtual std::string class_name() const noexcept;

    public:
        static std::vector<std::string> loggable_fields() noexcept;

    public:
        virtual std::vector<std::string> field_names() const noexcept;
        virtual types::Value get_field_as_value(const std::string &field_name) const;

    public:
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;
        void to_stream(std::ostream &stream) const override;

    private:
        dt::TimePoint timepoint_;
        types::KeyValueMap attributes_;
    };

}  // namespace core::types
