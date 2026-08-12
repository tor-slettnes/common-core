/// -*- c++ -*-
//==============================================================================
/// @file variant-value.h++
/// @brief Generic `Value` type
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "variant-types.h++"
#include "types/streamable.h++"
#include "string/convert.h++"

#include <type_traits>
#include <filesystem>

namespace cc::core::types
{
    //==========================================================================
    /// @class Value
    /// @brief A general purpose variant value.
    ///
    class Value : public ValueBase, public Streamable
    {
        using Super = ValueBase;

    public:
        using Super::Super;

        Value();
        explicit Value(bool value);
        Value(std::uint8_t value);
        Value(std::uint16_t value);
        Value(std::uint32_t value);
        Value(std::uint64_t value);
        Value(std::int8_t value);
        Value(std::int16_t value);
        Value(std::int32_t value);
        Value(std::int64_t value);
        Value(const char* cstring);
        Value(const StringPtr& string);
        Value(const std::string& string);
        Value(std::string&& string);
        Value(const std::string_view& view);
        Value(const std::filesystem::path& path);
        Value(const std::vector<Byte>& bytes);
        Value(const BytesPtr& bytes);

        Value(const ValueListPtr& list);
        Value(const ValueList& list);
        Value(ValueList&& list);

        Value(const TaggedValueListPtr& tvlist);
        Value(const TaggedValueList& tvlist);
        Value(TaggedValueList&& tvlist);

        Value(const KeyValueMapPtr& kvmap);
        Value(const KeyValueMap& kvmap);
        Value(KeyValueMap&& kvmap);

        // Additional constructor to allow std::optional values
        template <class T>
        Value(const std::optional<T>& opt)
            : Value(opt ? Value(*opt) : Value())
        {
        }

        template <class T>
        Value(std::optional<T>&& opt)
            : Value(opt ? Value(std::move(*opt)) : Value())
        {
        }

        virtual bool operator==(const Value& other) const;
        void clear();
        void reset();

        /// Return the ValueType enumeration corresponding to this value.
        ValueType type() const noexcept;
        std::string type_name() const;
        bool empty() const noexcept;
        operator bool() const noexcept;
        bool has_type() const noexcept;
        bool has_nonempty_value() const noexcept;
        const Value& nonempty_value() const noexcept;

        bool is_simple() const noexcept;
        bool is_bool() const noexcept;
        bool is_char() const noexcept;
        bool is_numeric() const noexcept;
        bool is_integral() const noexcept;
        bool is_uint() const noexcept;
        bool is_sint() const noexcept;
        bool is_float() const noexcept;
        bool is_real() const noexcept;
        bool is_complex() const noexcept;
        bool is_text() const noexcept;
        bool is_string() const noexcept;
        bool is_bytevector() const noexcept;
        bool is_bytesequence() const noexcept;
        bool is_time() const noexcept;
        bool is_timepoint() const noexcept;
        bool is_duration() const noexcept;

        bool is_composite() const noexcept;
        bool is_mappable() const noexcept;
        bool is_sequence() const noexcept;
        bool is_valuelist() const noexcept;
        bool is_kvmap() const noexcept;
        bool is_tvlist() const noexcept;

        bool as_bool(bool fallback = false) const noexcept;
        char as_char(char fallback = '\0') const noexcept;
        unsigned short as_ushort(unsigned short fallback = 0) const noexcept;
        unsigned int as_uint(unsigned int fallback = 0) const noexcept;
        unsigned long as_ulong(unsigned long fallback = 0) const noexcept;
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
        largest_real as_real(largest_real fallback = 0.0) const noexcept;
        largest_real as_imag(largest_real fallback = 0.0) const noexcept;
        float as_float(float fallback = 0.0) const noexcept;
        double as_double(double fallback = 0.0) const noexcept;
        complex as_complex(const complex& fallback = {0.0, 0.0}) const noexcept;

        dt::TimePoint as_timepoint(
            bool assume_local = true,
            const dt::TimePoint& fallback = {}) const noexcept;

        dt::TimePoint as_timepoint(
            const std::optional<int>& decimal_exponent,
            bool assume_local = true,
            const dt::TimePoint& fallback = {}) const noexcept;

        dt::Duration as_duration(
            const dt::Duration& fallback = {}) const noexcept;

        dt::Duration as_duration(
            int decimal_exponent,
            const dt::Duration& fallback = {}) const noexcept;

        dt::Duration as_duration(
            double multiplier,
            const dt::Duration& fallback = {}) const noexcept;

        std::string as_string() const noexcept;
        ByteVector as_bytevector(const ByteVector& fallback = {}) const noexcept;

        ValueList as_valuelist() const noexcept;
        ValueList as_valuelist(const ValueList& fallback) const noexcept;
        ValueListPtr as_valuelist_ptr() const noexcept;

        TaggedValueList as_tvlist() const noexcept;
        TaggedValueList as_tvlist(const TaggedValueList& fallback) const noexcept;
        TaggedValueListPtr as_tvlist_ptr() const noexcept;

        KeyValueMap as_kvmap() const noexcept;
        KeyValueMap as_kvmap(const KeyValueMap& fallback) const noexcept;
        KeyValueMapPtr as_kvmap_ptr() const noexcept;

        std::optional<bool> try_as_bool() const noexcept;
        std::optional<char> try_as_char() const noexcept;
        std::optional<unsigned short> try_as_ushort() const noexcept;
        std::optional<unsigned int> try_as_uint() const noexcept;
        std::optional<unsigned long> try_as_ulong() const noexcept;
        std::optional<std::uint8_t> try_as_uint8() const noexcept;
        std::optional<std::uint16_t> try_as_uint16() const noexcept;
        std::optional<std::uint32_t> try_as_uint32() const noexcept;
        std::optional<std::uint64_t> try_as_uint64() const noexcept;
        std::optional<largest_uint> try_as_largest_uint() const noexcept;
        std::optional<short> try_as_sshort() const noexcept;
        std::optional<int> try_as_sint() const noexcept;
        std::optional<long> try_as_slong() const noexcept;
        std::optional<std::int8_t> try_as_sint8() const noexcept;
        std::optional<std::int16_t> try_as_sint16() const noexcept;
        std::optional<std::int32_t> try_as_sint32() const noexcept;
        std::optional<std::int64_t> try_as_sint64() const noexcept;
        std::optional<largest_sint> try_as_largest_sint() const noexcept;
        std::optional<largest_real> try_as_real() const noexcept;
        std::optional<largest_real> try_as_imag() const noexcept;
        std::optional<float> try_as_float() const noexcept;
        std::optional<double> try_as_double() const noexcept;
        std::optional<complex> try_as_complex() const noexcept;

        std::optional<dt::TimePoint> try_as_timepoint(
            bool assume_local = true) const noexcept;

        std::optional<dt::TimePoint> try_as_timepoint(
            const std::optional<int>& decimal_exponent,
            bool assume_local = true) const noexcept;

        std::optional<dt::Duration> try_as_duration() const noexcept;
        std::optional<dt::Duration> try_as_duration(double multiplier) const noexcept;
        std::optional<dt::Duration> try_as_duration(int decimal_exponent) const noexcept;

        /// @return
        ///    Constant reference to the contained string if applicable,
        ///    otherwise to a static empty string.
        const std::string& get_string() const;

        /// @return
        ///    Constant reference to the contained byte vector if applicable,
        ///    otherwise to a static empty byte vector.
        const ByteVector& get_bytevector() const;

        /// @return
        ///    Constant reference to the contained ValueList if applicable,
        ///    otherwise to a static empty ValueList.
        const ValueList& get_valuelist() const;

        /// @return
        ///    Constant reference to the contained TaggedValueList if applicable,
        ///    otherwise to a static empty TaggedValueList.
        const TaggedValueList& get_tvlist() const;

        /// @return
        ///    Constant reference to the contained KeyValueMap if applicable,
        ///    otherwise to a static empty TaggedValueList.
        const KeyValueMap& get_kvmap() const;

        /// @return
        ///    `shared_ptr` reference to the contained ByteVector if applicable,
        ///    otherwise an empty `shared_ptr`.
        BytesPtr get_bytevector_ptr() const noexcept;

        /// @return
        ///    `shared_ptr` reference to the contained ValueList if applicable,
        ///    otherwise an empty `shared_ptr`.
        ValueListPtr get_valuelist_ptr() const noexcept;

        /// @return
        ///    `shared_ptr` reference to the contained TaggedValueList if
        ///    applicable, otherwise an empty `shared_ptr`.
        TaggedValueListPtr get_tvlist_ptr() const noexcept;

        /// @return
        ///    `shared_ptr` reference to the contained KeyValueMap if
        ///    applicable, otherwise an empty `shared_ptr`.
        KeyValueMapPtr get_kvmap_ptr() const noexcept;

        Value front(
            const Value& fallback = {}) const noexcept;

        Value back(
            const Value& fallback = {}) const noexcept;

        Value get(
            const std::string& key,
            const Value& fallback = {},
            bool ignoreCase = false) const noexcept;

        Value get(
            const char* key,
            const Value& fallback = {},
            bool ignoreCase = false) const noexcept;

        Value get(
            const int index,
            const Value& fallback = {}) const noexcept;

        Value get(
            const uint index,
            const Value& fallback = {}) const noexcept;

        Value get_nonempty(
            const std::string& key,
            const Value& fallback = {},
            bool ignoreCase = false) const noexcept;

        Value get_any_of(
            const std::vector<std::string>& candidates,
            const Value& fallback = {},
            bool ignoreCase = false) const noexcept;

        Value get_nested(
            const std::vector<std::string>& path,
            const Value& fallback = {},
            bool ignoreCase = false) const noexcept;

        std::optional<Value> try_get(
            const std::string& key,
            bool ignoreCase = false) const noexcept;

        std::optional<Value> try_get(
            const char* key,
            bool ignoreCase = false) const noexcept;

        std::optional<Value> try_get(
            const int index) const noexcept;

        std::optional<Value> try_get(
            const uint index) const noexcept;

        std::optional<Value> try_get_nonempty(
            const std::string& key,
            bool ignoreCase = false) const noexcept;

        std::optional<Value> try_get_any_of(
            const std::vector<std::string>& candidates,
            bool ignoreCase = false) const noexcept;

        std::optional<Value> try_get_nested(
            const std::vector<std::string>& path,
            bool ignoreCase = false) const noexcept;

        Value& operator[](const char* key);
        Value& operator[](const std::string& key);
        Value& operator[](const uint index);
        Value& operator[](const int index);

        Value deepcopy() const;

    public:
        /// Send a readable representation of this value to an output stream
        void to_stream(std::ostream& stream) const override;
        void to_literal_stream(std::ostream& stream) const override;

    public:
        /// Parse and return a new Value instance from a literal string.
        /// Some examples:
        ///  - ``                 becomes an "empty" value (std::monostate)
        ///  - `false` or `true`  becomes a boolean
        ///  - `"some string"`    becomes a string
        ///  - `1234` or `0x1234` becomes an unsigned integer
        ///  - `+1234` or `-1234` becomes a signed interger
        ///  - `3.14159` or `3.`  becomes a floating point number
        ///  - `%base64string`    becomes a ByteVector instance
        ///  - `2020-01-01T00:00:00.000` becomes a dt::TimePoint instance
        ///  - `00:00:00.000`    becomes a dt::Duration instance
        static Value from_literal(const std::string_view& literal);

    public:
        // Convencience wrapper around std::get<T>(*this)
        template <class T>
        inline const T& get() const;

        // Convencience wrapper around std::get_if<T>(*this)
        template <class T, std::enable_if_t<!std::is_same_v<T, std::string>, bool> = false>
        inline const T* get_if() const;

        template <class T, std::enable_if_t<std::is_same_v<T, std::string>, bool> = true>
        inline const T* get_if() const;

        // Convencience wrapper around std::get_if<T>(*this)
        template <class T>
        inline const T* get_if_nonempty() const;

        template <class T>
        inline T& emplace_from(const Value& other);

        /// Returns true if the value type is contained in any of the provdied
        /// template arguments.
        template <class... Ts>
        inline bool holdsAnyOf() const noexcept;

        /// Try to cast value to a desired type
        template <class T>
        std::optional<T> try_numeric_cast() const;

        /// Try to cast value to a desired type, or return `fallback` otherwise.
        template <class T>
        T numeric_cast(T fallback = 0) const;

        // Try to convert the value to a specific type
        template <class T>
        std::optional<T> try_convert_to() const;

        /// Convert value to a desired value via istream `>>' operator
        template <class T>
        T convert_to(const T& fallback = {}) const;

        template <class T>
        static Value create_from(const T& input);
    };

    //--------------------------------------------------------------------------
    // Non-member functions

    template <class T>
    Value operator<<(Value& value, const T& input);

    //--------------------------------------------------------------------------
    // Non-member contents

    extern const ValueBase emptyvalue;

}  // namespace cc::core::types

#include "variant-value.i++"
