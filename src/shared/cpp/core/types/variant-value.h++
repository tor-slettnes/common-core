/// -*- c++ -*-
//==============================================================================
/// @file variant-value.h++
/// @brief Generic `Value` type
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "variant-types.h++"
#include "types/streamable.h++"
#include "string/convert.h++"

namespace cc::types
{
    //==========================================================================
    /// \class Value
    /// \brief A general purpose variant value.
    ///
    class Value : public ValueBase, public Streamable
    {
        using Super = ValueBase;

    public:
        using Super::Super;

        Value(std::uint8_t value);
        Value(std::uint16_t value);
        Value(std::uint32_t value);
        Value(std::uint64_t value);
        Value(std::int8_t value);
        Value(std::int16_t value);
        Value(std::int32_t value);
        Value(std::int64_t value);
        Value(char *cstring);
        Value(const std::vector<Byte> &bytes);

        Value(const ValueList &list);
        Value(ValueList &&list);

        Value(const KeyValueMap &kvmap);
        Value(KeyValueMap &&kvmap);

        Value(const TaggedValueList &tvlist);
        Value(TaggedValueList &&tvlist);

        // Additional constructor to allow std::optional values
        template <class T>
        Value(const std::optional<T> &opt)
            : Value(opt.has_value() ? Value(opt.value()) : Value())
        {
        }

        void clear();
        void reset();

        /// Return the ValueType enumeration corresponding to this value.
        ValueType type() const noexcept;
        std::string type_name() const;
        bool empty() const noexcept;
        operator bool() const noexcept;
        bool has_value() const noexcept;
        bool is_simple() const noexcept;
        bool is_composite() const noexcept;
        bool is_bool() const noexcept;
        bool is_char() const noexcept;
        bool is_uint() const noexcept;
        bool is_sint() const noexcept;
        bool is_numeric() const noexcept;
        bool is_integral() const noexcept;
        bool is_float() const noexcept;
        bool is_real() const noexcept;
        bool is_complex() const noexcept;
        bool is_string() const noexcept;
        bool is_bytearray() const noexcept;
        bool is_timepoint() const noexcept;
        bool is_duration() const noexcept;
        bool is_valuelist() const noexcept;
        bool is_kvmap() const noexcept;
        bool is_tvlist() const noexcept;
        bool as_bool(bool fallback = false) const noexcept;
        char as_char(char fallback = '\0') const noexcept;
        ushort as_ushort(ushort fallback = 0) const noexcept;
        uint as_uint(uint fallback = 0) const noexcept;
        ulong as_ulong(ulong fallback = 0) const noexcept;
        std::uint8_t as_uint8(std::uint8_t fallback = 0) const noexcept;
        std::uint16_t as_uint16(std::uint16_t fallback = 0) const noexcept;
        std::uint32_t as_uint32(std::uint32_t fallback = 0) const noexcept;
        std::uint64_t as_uint64(std::uint64_t fallback = 0) const noexcept;
        largest_uint as_largest_uint(largest_uint fallback = 0) const noexcept;
        short as_sshort(short fallback = 0) const noexcept;
        int as_sint(int fallback = 0) const noexcept;
        long as_slong(long fallback = 0) const noexcept;
        std::int8_t as_sint8(std::int8_t fallback = 0) const noexcept;
        std::int16_t as_sint16(std::int16_t fallback = 0) const noexcept;
        std::int32_t as_sint32(std::int32_t fallback = 0) const noexcept;
        std::int64_t as_sint64(std::int64_t fallback = 0) const noexcept;
        largest_sint as_largest_sint(largest_sint fallback = 0) const noexcept;
        double as_real(double fallback = 0.0) const noexcept;
        double as_imag(double fallback = 0.0) const noexcept;
        float as_float(float fallback = 0.0) const noexcept;
        double as_double(double fallback = 0.0) const noexcept;
        complex as_complex(const complex &fallback = {0.0, 0.0}) const noexcept;
        std::string as_string() const noexcept;
        ByteArray as_bytearray(const ByteArray &fallback = {}) const noexcept;
        dt::TimePoint as_timepoint(const dt::TimePoint &fallback = {}) const noexcept;
        dt::Duration as_duration(const dt::Duration &fallback = {}) const noexcept;

        ValueList as_valuelist() const noexcept;
        ValueList as_valuelist(const ValueList &fallback) const noexcept;

        KeyValueMap as_kvmap() const noexcept;
        KeyValueMap as_kvmap(const KeyValueMap &fallback) const noexcept;

        TaggedValueList as_tvlist() const noexcept;
        TaggedValueList as_tvlist(const TaggedValueList &fallback) const noexcept;

        ValueListRef get_valuelist() const noexcept;
        KeyValueMapRef get_kvmap() const noexcept;
        TaggedValueListRef get_tvlist() const noexcept;

        const Value &get(const std::string &key, const Value &fallback = {}) const noexcept;
        const Value &get(const uint index, const Value &fallback = {}) const noexcept;
        const Value &get(const int index, const Value &fallback = {}) const noexcept;

        // void replace_from(const Value &other);

        // Convencience wrapper around std::get<T>(*this)
        template <class T>
        inline const T &get() const
        {
            return std::get<T>(*this);
        }

        // Convencience wrapper around std::get<T>(*this)
        template <class T>
        inline const T *get_if() const
        {
            return std::get_if<T>(this);
        }

        template <class T>
        inline T &emplace_from(const Value &other)
        {
            return this->emplace<T>(std::get<T>(other));
        }

        /// Returns true if the value type is contained in any of the provdied
        /// template arguments.
        template <class... Ts>
        inline bool holdsAnyOf() const noexcept
        {
            return (std::holds_alternative<Ts>(*this) || ...);
        }

        /// Cast value to a desired type
        template <class T>
        T numeric_cast(T fallback = 0) const
        {
            switch (this->type())
            {
            case ValueType::NONE:
                return fallback;

            case ValueType::BOOL:
                return static_cast<T>(this->get<bool>());

            case ValueType::CHAR:
                return static_cast<T>(this->get<char>());

            case ValueType::UINT:
                return static_cast<T>(this->get<largest_uint>());

            case ValueType::SINT:
                return static_cast<T>(this->get<largest_sint>());

            case ValueType::REAL:
                return static_cast<T>(this->get<double>());

            case ValueType::COMPLEX:
                return static_cast<T>(this->get<complex>().real());

            case ValueType::STRING:
                return str::convert_to<T>(this->get<std::string>(), {});

            case ValueType::BYTEARRAY:
                return this->get<ByteArray>().unpack<T>();

            case ValueType::TIMEPOINT:
                return (T)dt::to_double(this->get<dt::TimePoint>().time_since_epoch());

            case ValueType::DURATION:
                return (T)dt::to_double(this->get<dt::Duration>());

            default:
                return fallback;
            }
        }

        /// Parse and return a new Value instance from a literal string.
        /// Some examples (where the literal is shown inside the [brackets]):
        ///  - []                 becomes an "empty" value (std::monostate)
        ///  - [false] or [true]  becomes a boolean
        ///  - ['c']              becomes a character
        ///  - ["some string"]    becomes a string
        ///  - [1234] or [0x1234] becomes an unsigned integer
        ///  - [+1234] or [-1234] becomes a signed interger
        ///  - [3.14159] or [3.]  becomes a floating point number
        ///  - [%base64string]    becomes a ByteArray instance
        ///  - [2020-01-01T00:00:00.000] becomes a dt::TimePoint instance
        ///  - [00:00:00.000]    becomes a dt::Duration instance
        static Value from_literal(const std::string &literal);

    public:
        /// Send a readable representation of this value to an output stream
        void to_stream(std::ostream &stream) const override;
        void to_literal_stream(std::ostream &stream) const override;

    protected:
        /// Determine value type from a literal representation
        static ValueType literal_type(const std::string &literal);
        static ValueType implicit_type(const ValueBase &base) noexcept;

        // protected:
        //     std::optional<ValueType> type_;
    };

}  // namespace cc::types
