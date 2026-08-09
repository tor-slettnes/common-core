// -*- c++ -*-
//==============================================================================
/// @file variant-value.c++
/// @brief Generic `Value` type
/// @author Tor Slettnes
//==============================================================================

#include "variant-value.h++"
#include "variant-list.h++"
#include "variant-kvmap.h++"
#include "variant-tvlist.h++"
#include "create-shared.h++"
#include "string/convert.h++"
#include "parsers/json/reader.h++"

#include <sstream>

namespace cc::core::types
{
    Value::Value()
        : ValueBase(nullvalue)
    {
    }

    Value::Value(bool value)
        : ValueBase(value)
    {
    }

    Value::Value(std::uint8_t value)
        : ValueBase(static_cast<largest_uint>(value))
    {
    }

    Value::Value(std::uint16_t value)
        : ValueBase(static_cast<largest_uint>(value))
    {
    }

    Value::Value(std::uint32_t value)
        : ValueBase(static_cast<largest_uint>(value))
    {
    }

    Value::Value(std::uint64_t value)
        : ValueBase(static_cast<largest_uint>(value))
    {
    }

    Value::Value(std::int8_t value)
        : ValueBase(static_cast<largest_sint>(value))
    {
    }

    Value::Value(std::int16_t value)
        : ValueBase(static_cast<largest_sint>(value))
    {
    }

    Value::Value(std::int32_t value)
        : ValueBase(static_cast<largest_sint>(value))
    {
    }

    Value::Value(std::int64_t value)
        : ValueBase(static_cast<largest_sint>(value))
    {
    }

    Value::Value(const char* cstring)
        : ValueBase(std::make_shared<std::string>(cstring))
    {
    }

    Value::Value(const StringPtr& ptr)
        : ValueBase(ptr)
    {
    }

    Value::Value(const std::string& string)
        : ValueBase(std::make_shared<std::string>(string))
    {
    }

    Value::Value(std::string&& string)
        : ValueBase(std::make_shared<std::string>(std::move(string)))
    {
    }

    Value::Value(const std::string_view& view)
        : ValueBase(std::make_shared<std::string>(view.begin(), view.end()))
    {
    }

    Value::Value(const std::filesystem::path& path)
        : ValueBase(std::make_shared<std::string>(path.string()))
    {
    }

    Value::Value(const std::vector<Byte>& bytes)
        : ValueBase(std::make_shared<ByteVector>(bytes.begin(), bytes.end()))
    {
    }

    Value::Value(const BytesPtr& bytes)
        : ValueBase(bytes)
    {
    }

    Value::Value(const ValueListPtr& list)
        : ValueBase(list ? list : std::make_shared<ValueList>())
    {
    }

    Value::Value(const ValueList& list)
        : ValueBase(std::make_shared<ValueList>(list))
    {
    }

    Value::Value(ValueList&& list)
        : ValueBase(std::make_shared<ValueList>(std::move(list)))
    {
    }

    Value::Value(const TaggedValueListPtr& tvlist)
        : ValueBase(tvlist ? tvlist : std::make_shared<TaggedValueList>())
    {
    }

    Value::Value(const TaggedValueList& tvlist)
        : ValueBase(std::make_shared<TaggedValueList>(tvlist))
    {
    }

    Value::Value(TaggedValueList&& tvlist)
        : ValueBase(std::make_shared<TaggedValueList>(std::move(tvlist)))
    {
    }

    Value::Value(const KeyValueMapPtr& kvmap)
        : ValueBase(kvmap ? kvmap : std::make_shared<KeyValueMap>())
    {
    }

    Value::Value(const KeyValueMap& kvmap)
        : ValueBase(std::make_shared<KeyValueMap>(kvmap))
    {
    }

    Value::Value(KeyValueMap&& kvmap)
        : ValueBase(std::make_shared<KeyValueMap>(std::move(kvmap)))
    {
    }

    bool Value::operator==(const Value& other) const
    {
        if (this->type() == other.type())
        {
            switch (this->type())
            {
            case ValueType::NONE:
                return true;

            case ValueType::BOOL:
                return this->get<bool>() == other.get<bool>();

            case ValueType::CHAR:
                return this->get<char>() == other.get<char>();

            case ValueType::UINT:
                return this->get<largest_uint>() == other.get<largest_uint>();

            case ValueType::SINT:
                return this->get<largest_sint>() == other.get<largest_sint>();

            case ValueType::REAL:
                return this->get<largest_real>() == other.get<largest_real>();

            case ValueType::COMPLEX:
                return this->get<complex>() == other.get<complex>();

            case ValueType::STRING:
                return this->get_string() == other.get_string();

            case ValueType::BYTEVECTOR:
                return this->get_bytevector() == other.get_bytevector();

            case ValueType::TIMEPOINT:
                return this->get<dt::TimePoint>() == other.get<dt::TimePoint>();

            case ValueType::DURATION:
                return this->get<dt::Duration>() == other.get<dt::Duration>();

            case ValueType::VALUELIST:
                return (this->get_valuelist() == other.get_valuelist());

            case ValueType::TVLIST:
                return (this->get_tvlist() == other.get_tvlist());

            case ValueType::KVMAP:
                return (this->get_kvmap() == other.get_kvmap());

            default:
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    void Value::clear()
    {
        this->emplace<NullValueType>(nullvalue);
    }

    void Value::reset()
    {
        this->emplace<NullValueType>(nullvalue);
    }

    ValueType Value::type() const noexcept
    {
        return this->valueless_by_exception()
                 ? ValueType::NONE
                 : static_cast<ValueType>(this->index());
    }

    std::string Value::type_name() const
    {
        return TypeNames.to_string(this->type(), "(Unknown value type)");
    }

    bool Value::empty() const noexcept
    {
        return this->type() == ValueType::NONE;
    }

    Value::operator bool() const noexcept
    {
        return this->type() != ValueType::NONE;
    }

    bool Value::has_type() const noexcept
    {
        return this->type() != ValueType::NONE;
    }

    bool Value::has_nonempty_value() const noexcept
    {
        switch (this->type())
        {
        case ValueType::NONE:
            return false;

        case ValueType::CHAR:
            return this->get<char>() != '\0';

        case ValueType::STRING:
            return !this->get_string().empty();

        case ValueType::BYTEVECTOR:
            return !this->get_bytevector().empty();

        case ValueType::TIMEPOINT:
            return this->get<dt::TimePoint>() != dt::epoch;

        case ValueType::DURATION:
            return this->get<dt::Duration>() != dt::Duration::zero();

        case ValueType::VALUELIST:
            return !this->get<ValueListPtr>()->empty();

        case ValueType::TVLIST:
            return !this->get<TaggedValueListPtr>()->empty();

        case ValueType::KVMAP:
            return !this->get<KeyValueMapPtr>()->empty();

        default:
            return true;
        }
    }

    bool Value::is_simple() const noexcept
    {
        return !this->is_composite();
    }

    bool Value::is_bool() const noexcept
    {
        return this->holdsAnyOf<bool>();
    }

    bool Value::is_char() const noexcept
    {
        return this->holdsAnyOf<char>();
    }

    bool Value::is_numeric() const noexcept
    {
        return this->holdsAnyOf<largest_uint, largest_sint, largest_real, complex>();
    }

    bool Value::is_integral() const noexcept
    {
        return this->holdsAnyOf<largest_uint, largest_sint>();
    }

    bool Value::is_uint() const noexcept
    {
        return this->holdsAnyOf<largest_uint>();
    }

    bool Value::is_sint() const noexcept
    {
        return this->holdsAnyOf<largest_sint>();
    }

    bool Value::is_real() const noexcept
    {
        return this->holdsAnyOf<largest_uint, largest_sint, largest_real>();
    }

    bool Value::is_float() const noexcept
    {
        return this->holdsAnyOf<largest_real>();
    }

    bool Value::is_complex() const noexcept
    {
        return this->holdsAnyOf<complex>();
    }

    bool Value::is_text() const noexcept
    {
        return this->holdsAnyOf<char, StringPtr>();
    }

    bool Value::is_string() const noexcept
    {
        return this->holdsAnyOf<StringPtr>();
    }

    bool Value::is_bytevector() const noexcept
    {
        return this->holdsAnyOf<BytesPtr>();
    }

    bool Value::is_bytesequence() const noexcept
    {
        return this->holdsAnyOf<StringPtr, BytesPtr>();
    }

    bool Value::is_time() const noexcept
    {
        return this->holdsAnyOf<dt::TimePoint, dt::Duration>();
    }

    bool Value::is_timepoint() const noexcept
    {
        return this->holdsAnyOf<dt::TimePoint>();
    }

    bool Value::is_duration() const noexcept
    {
        return this->holdsAnyOf<dt::Duration>();
    }

    bool Value::is_composite() const noexcept
    {
        return this->holdsAnyOf<ValueListPtr, TaggedValueListPtr, KeyValueMapPtr>();
    }

    bool Value::is_mappable() const noexcept
    {
        return this->holdsAnyOf<KeyValueMapPtr, TaggedValueListPtr>();
    }

    bool Value::is_sequence() const noexcept
    {
        return this->holdsAnyOf<ValueListPtr, TaggedValueListPtr>();
    }

    bool Value::is_valuelist() const noexcept
    {
        return this->holdsAnyOf<ValueListPtr>();
    }

    bool Value::is_tvlist() const noexcept
    {
        return this->holdsAnyOf<TaggedValueListPtr>();
    }

    bool Value::is_kvmap() const noexcept
    {
        return this->holdsAnyOf<KeyValueMapPtr>();
    }

    //--------------------------------------------------------------------------
    // Getters with fallback values

    bool Value::as_bool(bool fallback) const noexcept
    {
        return this->try_as_bool().value_or(fallback);
    }

    char Value::as_char(char fallback) const noexcept
    {
        return this->try_as_char().value_or(fallback);
    }

    /// Return value as an unsigned integer
    unsigned short Value::as_ushort(unsigned short fallback) const noexcept
    {
        return this->try_as_ushort().value_or(fallback);
    }

    unsigned int Value::as_uint(unsigned int fallback) const noexcept
    {
        return this->try_as_uint().value_or(fallback);
    }

    unsigned long Value::as_ulong(unsigned long fallback) const noexcept
    {
        return this->try_as_ulong().value_or(fallback);
    }

    std::uint8_t Value::as_uint8(std::uint8_t fallback) const noexcept
    {
        return this->try_as_uint8().value_or(fallback);
    }

    std::uint16_t Value::as_uint16(std::uint16_t fallback) const noexcept
    {
        return this->try_as_uint16().value_or(fallback);
    }

    std::uint32_t Value::as_uint32(std::uint32_t fallback) const noexcept
    {
        return this->try_as_uint32().value_or(fallback);
    }

    std::uint64_t Value::as_uint64(std::uint64_t fallback) const noexcept
    {
        return this->try_as_uint64().value_or(fallback);
    }

    largest_uint Value::as_largest_uint(largest_uint fallback) const noexcept
    {
        return this->try_as_largest_uint().value_or(fallback);
    }

    /// Return value as a signed integer
    short Value::as_sshort(short fallback) const noexcept
    {
        return this->try_as_sshort().value_or(fallback);
    }

    int Value::as_sint(int fallback) const noexcept
    {
        return this->try_as_sint().value_or(fallback);
    }

    long Value::as_slong(long fallback) const noexcept
    {
        return this->try_as_slong().value_or(fallback);
    }

    std::int8_t Value::as_sint8(std::int8_t fallback) const noexcept
    {
        return this->try_as_sint8().value_or(fallback);
    }
    std::int16_t Value::as_sint16(std::int16_t fallback) const noexcept
    {
        return this->try_as_sint16().value_or(fallback);
    }

    std::int32_t Value::as_sint32(std::int32_t fallback) const noexcept
    {
        return this->try_as_sint32().value_or(fallback);
    }

    std::int64_t Value::as_sint64(std::int64_t fallback) const noexcept
    {
        return this->try_as_sint64().value_or(fallback);
    }

    largest_sint Value::as_largest_sint(largest_sint fallback) const noexcept
    {
        return this->try_as_largest_sint().value_or(fallback);
    }

    /// Return value as a floating point number
    largest_real Value::as_real(largest_real fallback) const noexcept
    {
        return this->try_as_real().value_or(fallback);
    }

    largest_real Value::as_imag(largest_real fallback) const noexcept
    {
        return this->try_as_imag().value_or(fallback);
    }

    float Value::as_float(float fallback) const noexcept
    {
        return this->numeric_cast<float>(fallback);
    }

    double Value::as_double(double fallback) const noexcept
    {
        return this->numeric_cast<double>(fallback);
    }

    complex Value::as_complex(const complex& fallback) const noexcept
    {
        return this->try_as_complex().value_or(fallback);
    }

    std::string Value::as_string() const noexcept
    {
        switch (this->type())
        {
        case ValueType::STRING:
            return this->get_string();

        default:
            return this->to_string();
        }
    }

    ByteVector Value::as_bytevector(const ByteVector& fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::NONE:
            return {};

        case ValueType::BOOL:
            return ByteVector::pack(this->get<bool>());

        case ValueType::CHAR:
            return ByteVector::pack(this->get<char>());

        case ValueType::UINT:
            return ByteVector::pack(this->get<largest_uint>());

        case ValueType::SINT:
            return ByteVector::pack(this->get<largest_sint>());

        case ValueType::REAL:
            return ByteVector::pack(this->get<largest_real>());

        case ValueType::COMPLEX:
            return ByteVector::pack(this->get<complex>());

        case ValueType::TIMEPOINT:
            return ByteVector::pack(this->get<dt::TimePoint>());

        case ValueType::DURATION:
            return ByteVector::pack(this->get<dt::Duration>());

        case ValueType::STRING:
            return ByteVector::from_string(this->get_string());

        case ValueType::BYTEVECTOR:
            return this->get_bytevector();

        default:
            return {};
        }
    }

    dt::TimePoint Value::as_timepoint(
        bool assume_local,
        const dt::TimePoint& fallback) const noexcept
    {
        return this->try_as_timepoint(assume_local).value_or(fallback);
    }

    dt::TimePoint Value::as_timepoint(
        const std::optional<int>& decimal_exponent,
        bool assume_local,
        const dt::TimePoint& fallback) const noexcept
    {
        return this->try_as_timepoint(decimal_exponent, assume_local).value_or(fallback);
    }

    dt::Duration Value::as_duration(const dt::Duration& fallback) const noexcept
    {
        return this->try_as_duration().value_or(fallback);
    }

    dt::Duration Value::as_duration(double multiplier,
                                    const dt::Duration& fallback) const noexcept
    {
        return this->try_as_duration(multiplier).value_or(fallback);
    }

    dt::Duration Value::as_duration(int decimal_exponent,
                                    const dt::Duration& fallback) const noexcept
    {
        return this->try_as_duration(decimal_exponent).value_or(fallback);
    }

    ValueList Value::as_valuelist() const noexcept
    {
        return this->as_valuelist({});
    }

    ValueList Value::as_valuelist(const ValueList& fallback) const noexcept
    {
        if (auto vlist = this->as_valuelist_ptr())
        {
            return *vlist;
        }
        else
        {
            return fallback;
        }
    }

    ValueListPtr Value::as_valuelist_ptr() const noexcept
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return this->get<ValueListPtr>();

        case ValueType::TVLIST:
            return this->get<TaggedValueListPtr>()->values_ptr();

        case ValueType::KVMAP:
            return this->get<KeyValueMapPtr>()->values_ptr();

        case ValueType::COMPLEX:
            std::make_shared<ValueList>(ValueList{this->as_real(), this->as_imag()});

        default:
            return {};
        }
    }

    TaggedValueList Value::as_tvlist() const noexcept
    {
        return this->as_tvlist({});
    }

    TaggedValueList Value::as_tvlist(const TaggedValueList& fallback) const noexcept
    {
        if (auto tvlist = this->as_tvlist_ptr())
        {
            return *tvlist;
        }
        else
        {
            return fallback;
        }
    }

    TaggedValueListPtr Value::as_tvlist_ptr() const noexcept
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return this->get<ValueListPtr>()->as_tvlist_ptr();

        case ValueType::KVMAP:
            return this->get<KeyValueMapPtr>()->as_tvlist_ptr();

        case ValueType::TVLIST:
            return this->get<TaggedValueListPtr>();

        case ValueType::COMPLEX:
            return std::make_shared<TaggedValueList>(TaggedValueList{
                {REAL_PART, this->as_real()},
                {IMAG_PART, this->as_imag()},
            });

        default:
            return {};
        }
    }

    KeyValueMap Value::as_kvmap() const noexcept
    {
        return this->as_kvmap({});
    }

    KeyValueMap Value::as_kvmap(const KeyValueMap& fallback) const noexcept
    {
        if (auto kvmap = this->as_kvmap_ptr())
        {
            return *kvmap;
        }
        else
        {
            return fallback;
        }
    }

    KeyValueMapPtr Value::as_kvmap_ptr() const noexcept
    {
        switch (this->type())
        {
        case ValueType::KVMAP:
            return this->get<KeyValueMapPtr>();

        case ValueType::TVLIST:
            return this->get<TaggedValueListPtr>()->as_kvmap_ptr();

        case ValueType::COMPLEX:
            return std::make_shared<KeyValueMap>(KeyValueMap{
                {REAL_PART, this->as_real()},
                {IMAG_PART, this->as_imag()},
            });

        default:
            return {};
        }
    }

    //--------------------------------------------------------------------------
    // Getters with optional return values

    std::optional<bool> Value::try_as_bool() const noexcept
    {
        switch (this->type())
        {
        case ValueType::NONE:
            return false;

        case ValueType::BOOL:
            return this->get<bool>();

        case ValueType::CHAR:
            switch (this->get<char>())
            {
            case 'f':
            case 'F':
            case 'n':
            case 'N':
            case '0':
            case '\0':
                return false;
            case 't':
            case 'T':
            case 'y':
            case 'Y':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return true;
            default:
                return {};
            }

        case ValueType::UINT:
        case ValueType::SINT:
        case ValueType::REAL:
            return this->try_numeric_cast<bool>();

        case ValueType::COMPLEX:
            return (this->as_real() != 0.0) || (this->as_imag() != 0.0);

        case ValueType::STRING:
            return str::try_convert_to<bool>(this->get_string());

        case ValueType::BYTEVECTOR:
            for (auto& byte : this->get_bytevector())
            {
                if (byte > 0)
                {
                    return true;
                }
            }
            return false;

        case ValueType::TIMEPOINT:
            return this->get<dt::TimePoint>() != dt::epoch;

        case ValueType::DURATION:
            return this->get<dt::Duration>() != dt::Duration::zero();

        case ValueType::VALUELIST:
            return this->get<ValueListPtr>()->size() > 0;

        case ValueType::TVLIST:
            return this->get<TaggedValueListPtr>()->size() > 0;

        case ValueType::KVMAP:
            return this->get<KeyValueMapPtr>()->size() > 0;

        default:
            return {};
        }
    }

    std::optional<char> Value::try_as_char() const noexcept
    {
        switch (this->type())
        {
        case ValueType::NONE:
            return {};

        case ValueType::CHAR:
            return this->get<char>();

        case ValueType::BOOL:
            return this->get<bool>() ? 't' : 'f';

        case ValueType::STRING:
            if (this->get_string().size() > 0)
            {
                return this->get_string().front();
            }
            else
            {
                return {};
            }

        case ValueType::BYTEVECTOR:
            if (this->get_bytevector().size() == sizeof(char))
            {
                return static_cast<const char>(this->get_bytevector().front());
            }
            else
            {
                return {};
            }

        default:
            return this->try_numeric_cast<char>();
        }
    }

    /// Return value as an unsigned integer
    std::optional<unsigned short> Value::try_as_ushort() const noexcept
    {
        return this->try_numeric_cast<ushort>();
    }

    std::optional<unsigned int> Value::try_as_uint() const noexcept
    {
        return this->try_numeric_cast<uint>();
    }

    std::optional<unsigned long> Value::try_as_ulong() const noexcept
    {
        return this->try_numeric_cast<ulong>();
    }

    std::optional<std::uint8_t> Value::try_as_uint8() const noexcept
    {
        return this->try_numeric_cast<std::uint8_t>();
    }

    std::optional<std::uint16_t> Value::try_as_uint16() const noexcept
    {
        return this->try_numeric_cast<std::uint16_t>();
    }

    std::optional<std::uint32_t> Value::try_as_uint32() const noexcept
    {
        return this->try_numeric_cast<std::uint32_t>();
    }

    std::optional<std::uint64_t> Value::try_as_uint64() const noexcept
    {
        return this->try_numeric_cast<std::uint64_t>();
    }

    std::optional<largest_uint> Value::try_as_largest_uint() const noexcept
    {
        return this->try_numeric_cast<largest_uint>();
    }

    /// Return value as a signed integer
    std::optional<short> Value::try_as_sshort() const noexcept
    {
        return this->try_numeric_cast<short>();
    }

    std::optional<int> Value::try_as_sint() const noexcept
    {
        return this->try_numeric_cast<int>();
    }

    std::optional<long> Value::try_as_slong() const noexcept
    {
        return this->try_numeric_cast<long>();
    }

    std::optional<std::int8_t> Value::try_as_sint8() const noexcept
    {
        return this->try_numeric_cast<std::int8_t>();
    }

    std::optional<std::int16_t> Value::try_as_sint16() const noexcept
    {
        return this->try_numeric_cast<std::int16_t>();
    }

    std::optional<std::int32_t> Value::try_as_sint32() const noexcept
    {
        return this->try_numeric_cast<std::int32_t>();
    }

    std::optional<std::int64_t> Value::try_as_sint64() const noexcept
    {
        return this->try_numeric_cast<std::int64_t>();
    }

    std::optional<largest_sint> Value::try_as_largest_sint() const noexcept
    {
        return this->try_numeric_cast<largest_sint>();
    }

    std::optional<largest_real> Value::try_as_real() const noexcept
    {
        return this->try_numeric_cast<largest_real>();
    }

    std::optional<largest_real> Value::try_as_imag() const noexcept
    {
        switch (this->type())
        {
        case ValueType::UINT:
        case ValueType::SINT:
        case ValueType::REAL:
            return 0.0;

        case ValueType::COMPLEX:
            return this->get<complex>().imag();

        default:
            return {};
        }
    }

    std::optional<float> Value::try_as_float() const noexcept
    {
        return this->try_numeric_cast<float>();
    }

    std::optional<double> Value::try_as_double() const noexcept
    {
        return this->try_numeric_cast<double>();
    }

    std::optional<complex> Value::try_as_complex() const noexcept
    {
        switch (this->type())
        {
        case ValueType::COMPLEX:
            return this->get<complex>();

        case ValueType::VALUELIST:
            if (auto list = this->get_valuelist_ptr())
            {
                if ((list->size() == 2) &&
                    list->front().is_numeric() &&
                    list->back().is_numeric())
                {
                    return complex(list->front().as_real(),
                                   list->back().as_real());
                }
            }
            break;

        case ValueType::KVMAP:
            if (auto kvmap = this->get_kvmap_ptr())
            {
                if (kvmap->count(REAL_PART) &&
                    kvmap->count(IMAG_PART))
                {
                    return complex(kvmap->get(REAL_PART).as_real(),
                                   kvmap->get(IMAG_PART).as_real());
                }
            }
            break;

        case ValueType::TVLIST:
            if (auto tvlist = this->get_tvlist_ptr())
            {
                if ((tvlist->size() == 2) &&
                    tvlist->front().is_numeric() &&
                    tvlist->back().is_numeric())
                {
                    return complex(tvlist->front().as_real(),
                                   tvlist->back().as_real());
                }
            }
            break;

        case ValueType::UINT:
        case ValueType::SINT:
        case ValueType::REAL:
            return complex(this->numeric_cast<largest_real>(), 0.0);

        default:
            break;
        }

        return {};
    }

    std::optional<dt::TimePoint> Value::try_as_timepoint(bool assume_local) const noexcept
    {
        return this->as_timepoint({}, assume_local);
    }

    std::optional<dt::TimePoint> Value::try_as_timepoint(
        const std::optional<int>& decimal_exponent,
        bool assume_local) const noexcept
    {
        switch (this->type())
        {
        case ValueType::TIMEPOINT:
            return this->get<dt::TimePoint>();

        case ValueType::DURATION:
            return dt::TimePoint(this->get<dt::Duration>());

        case ValueType::STRING:
            return dt::try_to_timepoint(
                this->get_string(),
                assume_local,
                decimal_exponent);

        case ValueType::UINT:
        case ValueType::SINT:
            return dt::int_to_timepoint(
                this->as_sint64(),
                decimal_exponent);

        case ValueType::REAL:
            return dt::double_to_timepoint(
                this->as_double(),
                decimal_exponent);

        case ValueType::BYTEVECTOR:
            try
            {
                return this->get_bytevector().unpack<dt::TimePoint>();
            }
            catch (const std::out_of_range&)
            {
                return {};
            }

        default:
            return {};
        }
    }

    std::optional<dt::Duration> Value::try_as_duration() const noexcept
    {
        switch (this->type())
        {
        case ValueType::REAL:
        case ValueType::COMPLEX:
        case ValueType::UINT:
        case ValueType::SINT:
            return dt::to_duration(this->as_real());

        case ValueType::STRING:
            return dt::try_to_duration(this->get_string());

        case ValueType::BYTEVECTOR:
            try
            {
                return this->get_bytevector().unpack<dt::Duration>();
            }
            catch (const std::out_of_range&)
            {
                return {};
            }

        case ValueType::TIMEPOINT:
            return this->get<dt::TimePoint>().time_since_epoch();

        case ValueType::DURATION:
            return this->get<dt::Duration>();

        default:
            return {};
        }
    }

    std::optional<dt::Duration> Value::try_as_duration(
        double multiplier) const noexcept
    {
        switch (this->type())
        {
        case ValueType::REAL:
        case ValueType::COMPLEX:
        case ValueType::UINT:
        case ValueType::SINT:
            return dt::to_duration(this->as_real(), multiplier);

        case ValueType::STRING:
            return dt::try_to_duration(this->get_string(), multiplier);

        default:
            return this->try_as_duration();
        }
    }

    std::optional<dt::Duration> Value::try_as_duration(
        int decimal_exponent) const noexcept
    {
        switch (this->type())
        {
        case ValueType::UINT:
        case ValueType::SINT:
            return dt::int_to_duration(this->as_sint64(), decimal_exponent);

        case ValueType::REAL:
        case ValueType::COMPLEX:
            return dt::to_duration(this->as_real(),
                                   std::pow(10, decimal_exponent));

        case ValueType::STRING:
            return dt::try_to_duration(this->get_string(),
                                       std::pow(10, decimal_exponent));

        default:
            return this->try_as_duration();
        }
    }

    //--------------------------------------------------------------------------
    // Non-converting getters

    const std::string& Value::get_string() const
    {
        if (auto ptr = this->get_string_ptr())
        {
            return *ptr;
        }
        else
        {
            static const std::string fallback;
            return fallback;
        }
    }

    const ByteVector& Value::get_bytevector() const
    {
        if (auto* bv = this->get_if<BytesPtr>())
        {
            return **bv;
        }
        else
        {
            static const ByteVector fallback;
            return fallback;
        }
    }

    const ValueList& Value::get_valuelist() const
    {
        if (auto* ptr = this->get_if<ValueListPtr>())
        {
            return **ptr;
        }
        else
        {
            static const ValueList fallback;
            return fallback;
        }
    }

    const TaggedValueList& Value::get_tvlist() const
    {
        if (auto* ptr = this->get_if<TaggedValueListPtr>())
        {
            return **ptr;
        }
        else
        {
            static const TaggedValueList fallback;
            return fallback;
        }
    }

    const KeyValueMap& Value::get_kvmap() const
    {
        if (auto* ptr = this->get_if<KeyValueMapPtr>())
        {
            return **ptr;
        }
        else
        {
            static const KeyValueMap fallback;
            return fallback;
        }
    }

    StringPtr Value::get_string_ptr() const noexcept
    {
        if (auto* ptr = this->get_if<StringPtr>())
        {
            return *ptr;
        }
        else
        {
            return {};
        }
    }

    BytesPtr Value::get_bytevector_ptr() const noexcept
    {
        if (auto* ptr = this->get_if<BytesPtr>())
        {
            return *ptr;
        }
        else
        {
            return {};
        }
    }

    ValueListPtr Value::get_valuelist_ptr() const noexcept
    {
        if (auto* ptr = this->get_if<ValueListPtr>())
        {
            return *ptr;
        }
        else
        {
            return {};
        }
    }

    TaggedValueListPtr Value::get_tvlist_ptr() const noexcept
    {
        if (auto* ptr = this->get_if<TaggedValueListPtr>())
        {
            return *ptr;
        }
        else
        {
            return {};
        }
    }

    KeyValueMapPtr Value::get_kvmap_ptr() const noexcept
    {
        if (auto* ptr = this->get_if<KeyValueMapPtr>())
        {
            return *ptr;
        }
        else
        {
            return {};
        }
    }

    Value Value::deepcopy() const
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return ValueList::create_shared(this->get<ValueListPtr>()->deepcopy());

        case ValueType::TVLIST:
            return TaggedValueList::create_shared(this->get<TaggedValueListPtr>()->deepcopy());

        case ValueType::KVMAP:
            return KeyValueMap::create_shared(this->get<KeyValueMapPtr>()->deepcopy());

        default:
            return *this;
        }
    }

    Value& Value::operator[](const char* key)
    {
        return this->operator[](std::string(key));
    }

    Value& Value::operator[](const std::string& key)
    {
        switch (this->type())
        {
        case ValueType::KVMAP:
            return (*std::get<KeyValueMapPtr>(*this))[key];

        case ValueType::TVLIST:
            return (*std::get<TaggedValueListPtr>(*this))[key];

        default:
            *this = KeyValueMap::create_shared();
            return (*std::get<KeyValueMapPtr>(*this))[key];
        }
    }

    Value& Value::operator[](const uint index)
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return std::get<ValueListPtr>(*this)->at(index);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->at(index).second;

        default:
            throw std::invalid_argument("Value instance is not indexable");
        }
    }

    Value& Value::operator[](const int index)
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return std::get<ValueListPtr>(*this)->at(index);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->at(index).second;

        default:
            throw std::invalid_argument("Value instance is not indexable");
        }
    }

    Value Value::front(const Value& fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return std::get<ValueListPtr>(*this)->front(fallback);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->front(fallback);

        default:
            return fallback;
        }
    }

    Value Value::back(const Value& fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return std::get<ValueListPtr>(*this)->back(fallback);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->back(fallback);

        default:
            return fallback;
        }
    }

    Value Value::get(
        const std::string& key,
        const Value& fallback,
        bool ignoreCase) const noexcept
    {
        return this->try_get(key, ignoreCase).value_or(fallback);
    }

    Value Value::get(
        const char* key,
        const Value& fallback,
        bool ignoreCase) const noexcept
    {
        return this->try_get(std::string(key), ignoreCase).value_or(fallback);
    }

    Value Value::get(
        const int index,
        const Value& fallback) const noexcept
    {
        return this->try_get(index).value_or(fallback);
    }

    Value Value::get(
        const uint index,
        const Value& fallback) const noexcept
    {
        return this->try_get(index).value_or(fallback);
    }

    Value Value::get_nonempty(
        const std::string& key,
        const Value& fallback,
        bool ignoreCase) const noexcept
    {
        return this->try_get_nonempty(key, ignoreCase).value_or(fallback);
    }

    Value Value::get_any_of(
        const std::vector<std::string>& candidates,
        const Value& fallback,
        bool ignoreCase) const noexcept
    {
        return this->try_get_any_of(candidates, ignoreCase).value_or(fallback);
    }

    Value Value::get_nested(
        const std::vector<std::string>& path,
        const Value& fallback,
        bool ignoreCase) const noexcept
    {
        return this->try_get_nested(path, ignoreCase).value_or(fallback);
    }

    std::optional<Value> Value::try_get(
        const std::string& key,
        bool ignoreCase) const noexcept
    {
        switch (this->type())
        {
        case ValueType::KVMAP:
            return std::get<KeyValueMapPtr>(*this)->try_get(key, ignoreCase);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->try_get(key, ignoreCase);

        default:
            return {};
        }
    }

    std::optional<Value> Value::try_get(
        const char* key,
        bool ignoreCase) const noexcept
    {
        return this->try_get(std::string(key), ignoreCase);
    }

    std::optional<Value> Value::try_get(
        const int index) const noexcept
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return std::get<ValueListPtr>(*this)->try_get(index);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->try_get(index);

        default:
            return {};
        }
    }

    std::optional<Value> Value::try_get(
        const uint index) const noexcept
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return std::get<ValueListPtr>(*this)->try_get(index);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->try_get(index);

        default:
            return {};
        }
    }

    std::optional<Value> Value::try_get_nonempty(
        const std::string& key,
        bool ignoreCase) const noexcept
    {
        switch (this->type())
        {
        case ValueType::KVMAP:
            return std::get<KeyValueMapPtr>(*this)->try_get_nonempty(key, ignoreCase);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->try_get_nonempty(key, ignoreCase);

        default:
            return {};
        }
    }

    std::optional<Value> Value::try_get_any_of(
        const std::vector<std::string>& candidates,
        bool ignoreCase) const noexcept
    {
        switch (this->type())
        {
        case ValueType::KVMAP:
            return std::get<KeyValueMapPtr>(*this)->try_get_any_of(candidates, ignoreCase);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->try_get_any_of(candidates, ignoreCase);

        default:
            return {};
        }
    }

    std::optional<Value> Value::try_get_nested(
        const std::vector<std::string>& path,
        bool ignoreCase) const noexcept
    {
        Value value = *this;
        for (const std::string& element : path)
        {
            if (const auto& next = value.try_get(element))
            {
                value = next.value();
            }
            else
            {
                return {};
            }
        }
        return value;
    }

    void Value::to_stream(std::ostream& stream) const
    {
        auto saved_state = stream::get_stream_state(stream);
        switch (this->type())
        {
        case ValueType::NONE:
            break;

        case ValueType::BOOL:
            stream << std::boolalpha << this->get<bool>();
            break;

        case ValueType::CHAR:
            stream << this->as_char();
            break;

        case ValueType::UINT:
            stream << this->as_largest_uint();
            break;

        case ValueType::SINT:
            stream << this->as_largest_sint();
            break;

        case ValueType::REAL:
            if (std::abs(this->as_real()) < 1e16)
            {
                stream << std::fixed;
            }
            stream << this->as_real();
            break;

        case ValueType::COMPLEX:
            stream << this->as_complex();
            break;

        case ValueType::STRING:
            stream << this->get_string();
            break;

        case ValueType::BYTEVECTOR:
            stream << this->get_bytevector();
            break;

        case ValueType::TIMEPOINT:
            stream << this->get<dt::TimePoint>();
            break;

        case ValueType::DURATION:
            stream << this->get<dt::Duration>();
            break;

        case ValueType::VALUELIST:
            this->get_valuelist().to_stream(stream);
            break;

        case ValueType::KVMAP:
            this->get_kvmap().to_stream(stream);
            break;

        case ValueType::TVLIST:
            this->get_tvlist().to_stream(stream);
            break;

        default:
            stream << "(Unknown value type " << this->type() << ")";
            break;
        }
        stream::set_stream_state(stream, saved_state);
    }

    void Value::to_literal_stream(std::ostream& stream) const
    {
        switch (this->type())
        {
        case ValueType::NONE:
            stream << "null";
            break;

        case ValueType::BYTEVECTOR:
            stream << "\"%" << this->get_bytevector().to_base64() << "%\"";
            break;

        case ValueType::TIMEPOINT:
            stream << std::quoted(dt::to_js_string(this->get<dt::TimePoint>()));
            break;

        case ValueType::DURATION:
            stream << std::quoted(dt::to_string(this->get<dt::Duration>()));
            break;

        case ValueType::STRING:
            str::to_literal(stream, this->get_string());
            break;

        case ValueType::VALUELIST:
            this->get_valuelist().to_literal_stream(stream);
            break;

        case ValueType::KVMAP:
            this->get_kvmap().to_literal_stream(stream);
            break;

        case ValueType::TVLIST:
            this->get_tvlist().to_literal_stream(stream);
            break;

        default:
            this->to_stream(stream);
            break;
        }
    }

    Value Value::from_literal(const std::string_view& literal)
    {
        static const std::regex rx_null("^(null|NULL|None)?$");
        static const std::regex rx_bytevector("^(['\"]?)%[[:alnum:]\\+/]+={0,2}%?\\1$");
        static const std::regex rx_literal_string("(['\"])((?:\\\\.|[^\\\\\\r\\n])*)\\1");
        static const std::regex rx_composite("^(?:\\{.*\\}|\\[.*\\])$");

        if (literal.empty())
        {
            return {};
        }
        else if (auto uint_value = str::try_convert_to<largest_uint>(literal))
        {
            return uint_value;
        }
        else if (auto sint_value = str::try_convert_to<largest_sint>(literal))
        {
            return sint_value;
        }
        else if (auto double_value = str::try_convert_to<double>(literal))
        {
            return double_value;
        }
        else if (auto bool_value = str::try_convert_to<bool>(literal))
        {
            return bool_value;
        }
        else if (auto complex_value = str::try_convert_to<complex>(literal))
        {
            return complex_value;
        }
        else if (auto duration_value = dt::try_to_duration(literal))
        {
            return duration_value;
        }
        else if (auto timepoint_value = dt::try_to_timepoint(literal))
        {
            return timepoint_value;
        }
        else if (std::regex_match(literal.begin(), literal.end(), rx_null))
        {
            return {};
        }
        else if (std::regex_match(literal.begin(), literal.end(), rx_bytevector))
        {
            return ByteVector::from_base64(str::unquoted(literal).substr(1));
        }
        else if (std::regex_match(literal.begin(), literal.end(), rx_literal_string))
        {
            return str::unquoted(literal);
        }
        else if (std::regex_match(literal.begin(), literal.end(), rx_composite))
        {
            return core::json::reader.decoded(literal);
        }
        else
        {
            return literal;
        }
    }

    //--------------------------------------------------------------------------
    // Non-member content

    const ValueBase emptyvalue;

}  // namespace cc::core::types
