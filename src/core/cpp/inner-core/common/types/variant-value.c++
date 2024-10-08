// -*- c++ -*-
//==============================================================================
/// @file variant-value.c++
/// @brief Generic `Value` type
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "variant-value.h++"
#include "variant-list.h++"
#include "variant-kvmap.h++"
#include "variant-tvlist.h++"

#include <sstream>

namespace core::types
{

    Value::Value()
        : ValueBase(nullvalue)
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

    Value::Value(char *cstring)
        : Value(std::string(cstring))
    {
    }

    Value::Value(const std::vector<Byte> &bytes)
        : Value(ByteVector(bytes.begin(), bytes.end()))
    {
    }

    Value::Value(const ValueList &list)
        : ValueBase(std::make_shared<ValueList>(list))
    {
    }

    Value::Value(ValueList &&list)
        : ValueBase(std::make_shared<ValueList>(std::move(list)))
    {
    }

    Value::Value(const KeyValueMap &kvmap)
        : ValueBase(std::make_shared<KeyValueMap>(kvmap))
    {
    }

    Value::Value(KeyValueMap &&kvmap)
        : ValueBase(std::make_shared<KeyValueMap>(std::move(kvmap)))
    {
    }

    Value::Value(const TaggedValueList &tvlist)
        : ValueBase(std::make_shared<TaggedValueList>(tvlist))
    {
    }

    Value::Value(TaggedValueList &&tvlist)
        : ValueBase(std::make_shared<TaggedValueList>(std::move(tvlist)))
    {
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
        return TypeNames.to_string(this->type()).value_or("(Unknown value type");
    }

    bool Value::empty() const noexcept
    {
        return this->holdsAnyOf<NullValueType>();
    }

    Value::operator bool() const noexcept
    {
        return !this->empty();
    }

    bool Value::has_value() const noexcept
    {
        return !this->empty();
    }

    bool Value::is_simple() const noexcept
    {
        return !this->is_composite();
    }

    bool Value::is_composite() const noexcept
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
        case ValueType::KVMAP:
        case ValueType::TVLIST:
            return true;

        default:
            return false;
        }
    }

    bool Value::is_bool() const noexcept
    {
        return this->holdsAnyOf<bool>();
    }

    bool Value::is_char() const noexcept
    {
        return this->holdsAnyOf<char>();
    }

    bool Value::is_uint() const noexcept
    {
        return this->holdsAnyOf<largest_uint>();
    }

    bool Value::is_sint() const noexcept
    {
        return this->holdsAnyOf<largest_sint>();
    }

    bool Value::is_integral() const noexcept
    {
        return this->holdsAnyOf<largest_uint, largest_sint>();
    }

    bool Value::is_numeric() const noexcept
    {
        return this->holdsAnyOf<largest_uint, largest_sint, double, complex>();
    }

    bool Value::is_float() const noexcept
    {
        return this->holdsAnyOf<double>();
    }

    bool Value::is_real() const noexcept
    {
        return this->holdsAnyOf<largest_uint, largest_sint, double>();
    }

    bool Value::is_complex() const noexcept
    {
        return this->holdsAnyOf<complex>();
    }

    bool Value::is_string() const noexcept
    {
        return this->holdsAnyOf<std::string>();
    }

    bool Value::is_bytevector() const noexcept
    {
        return this->holdsAnyOf<ByteVector>();
    }

    bool Value::is_timepoint() const noexcept
    {
        return this->holdsAnyOf<dt::TimePoint>();
    }

    bool Value::is_duration() const noexcept
    {
        return this->holdsAnyOf<dt::Duration>();
    }

    bool Value::is_valuelist() const noexcept
    {
        return this->holdsAnyOf<ValueListPtr>();
    }

    bool Value::is_kvmap() const noexcept
    {
        return this->holdsAnyOf<KeyValueMapPtr>();
    }

    bool Value::is_tvlist() const noexcept
    {
        return this->holdsAnyOf<TaggedValueListPtr>();
    }

    bool Value::as_bool(bool fallback) const noexcept
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
                return fallback;
            }

        case ValueType::COMPLEX:
            return (this->as_real() != 0.0) || (this->as_imag() != 0.0);

        case ValueType::STRING:
            return str::convert_to<bool>(this->get<std::string>(), fallback);

        case ValueType::BYTEVECTOR:
            for (auto &b : this->get<ByteVector>())
            {
                if (b > 0)
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

        case ValueType::KVMAP:
            return this->get<KeyValueMapPtr>()->size() > 0;

        case ValueType::TVLIST:
            return this->get<TaggedValueListPtr>()->size() > 0;

        default:
            return this->numeric_cast<bool>(fallback ? 1 : 0);
        }
    }

    char Value::as_char(char fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::NONE:
            return fallback;

        case ValueType::CHAR:
            return this->get<char>();

        case ValueType::BOOL:
            return this->get<bool>() ? 't' : 'f';

        case ValueType::STRING:
        {
            const std::string &s = this->get<std::string>();
            return s.size() ? s.front() : fallback;
        }

        case ValueType::BYTEVECTOR:
        {
            const ByteVector &b = this->get<ByteVector>();
            if (b.size() == sizeof(char))
                return *(const char *)(b.data());
            else
                return fallback;
        }

        default:
            return this->numeric_cast<char>(fallback);
        }
    }

    /// Return value as an unsigned integer
    unsigned short Value::as_ushort(unsigned short fallback) const noexcept
    {
        return this->numeric_cast<ushort>(fallback);
    }
    unsigned int Value::as_uint(unsigned int fallback) const noexcept
    {
        return this->numeric_cast<uint>(fallback);
    }
    unsigned long Value::as_ulong(unsigned long fallback) const noexcept
    {
        return this->numeric_cast<ulong>(fallback);
    }
    std::uint8_t Value::as_uint8(std::uint8_t fallback) const noexcept
    {
        return this->numeric_cast<std::uint8_t>(fallback);
    }
    std::uint16_t Value::as_uint16(std::uint16_t fallback) const noexcept
    {
        return this->numeric_cast<std::uint16_t>(fallback);
    }
    std::uint32_t Value::as_uint32(std::uint32_t fallback) const noexcept
    {
        return this->numeric_cast<std::uint32_t>(fallback);
    }
    std::uint64_t Value::as_uint64(std::uint64_t fallback) const noexcept
    {
        return this->numeric_cast<std::uint64_t>(fallback);
    }
    largest_uint Value::as_largest_uint(largest_uint fallback) const noexcept
    {
        return this->numeric_cast<largest_uint>(fallback);
    }

    /// Return value as a signed integer
    short Value::as_sshort(short fallback) const noexcept
    {
        return this->numeric_cast<short>(fallback);
    }
    int Value::as_sint(int fallback) const noexcept
    {
        return this->numeric_cast<int>(fallback);
    }
    long Value::as_slong(long fallback) const noexcept
    {
        return this->numeric_cast<long>(fallback);
    }
    std::int8_t Value::as_sint8(std::int8_t fallback) const noexcept
    {
        return this->numeric_cast<std::int8_t>(fallback);
    }
    std::int16_t Value::as_sint16(std::int16_t fallback) const noexcept
    {
        return this->numeric_cast<std::int16_t>(fallback);
    }
    std::int32_t Value::as_sint32(std::int32_t fallback) const noexcept
    {
        return this->numeric_cast<std::int32_t>(fallback);
    }
    std::int64_t Value::as_sint64(std::int64_t fallback) const noexcept
    {
        return this->numeric_cast<std::int64_t>(fallback);
    }

    largest_sint Value::as_largest_sint(largest_sint fallback) const noexcept
    {
        return this->numeric_cast<largest_sint>(fallback);
    }

    float Value::as_float(float fallback) const noexcept
    {
        return this->numeric_cast<float>(fallback);
    }

    double Value::as_double(double fallback) const noexcept
    {
        return this->numeric_cast<double>(fallback);
    }

    /// Return value as a floating point number
    double Value::as_real(double fallback) const noexcept
    {
        return this->numeric_cast<double>(fallback);
    }

    double Value::as_imag(double fallback) const noexcept
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
            return fallback;
        }
    }

    complex Value::as_complex(const complex &fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::COMPLEX:
            return this->get<complex>();

        case ValueType::VALUELIST:
        {
            auto list = this->get<ValueListPtr>();
            auto it = list->begin();
            return {
                list->get(0).as_real(),
                list->get(1).as_real(),
            };
        }

        case ValueType::KVMAP:
        {
            auto map = this->get<KeyValueMapPtr>();
            return {
                map->get(REAL_PART).as_real(),
                map->get(IMAG_PART).as_real(),
            };
        }

        case ValueType::TVLIST:
        {
            auto tvlist = this->get<TaggedValueListPtr>();
            return {
                tvlist->get(0).as_real(),
                tvlist->get(1).as_real(),
            };
        }

        case ValueType::UINT:
        case ValueType::SINT:
        case ValueType::REAL:
            return {this->numeric_cast<double>(), 0.0};

        default:
            return fallback;
        }
    }

    std::string Value::as_string() const noexcept
    {
        std::ostringstream out;
        switch (this->type())
        {
        case ValueType::TIMEPOINT:
            out << dt::to_js_string(this->get<dt::TimePoint>());
            break;

        default:
            out << *this;
            break;
        }
        return out.str();
    }

    ByteVector Value::as_bytevector(const ByteVector &fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::NONE:
            return fallback;

        case ValueType::BOOL:
            return ByteVector::pack(this->get<bool>());

        case ValueType::CHAR:
            return ByteVector::pack(this->get<char>());

        case ValueType::UINT:
            return ByteVector::pack(this->get<largest_uint>());

        case ValueType::SINT:
            return ByteVector::pack(this->get<largest_sint>());

        case ValueType::REAL:
            return ByteVector::pack(this->get<double>());

        case ValueType::COMPLEX:
            return ByteVector::pack(this->get<complex>());

        case ValueType::TIMEPOINT:
            return ByteVector::pack(this->get<dt::TimePoint>());

        case ValueType::DURATION:
            return ByteVector::pack(this->get<dt::Duration>());

        case ValueType::STRING:
            return ByteVector::from_string(this->get<std::string>());

        case ValueType::BYTEVECTOR:
            return this->get<ByteVector>();

            //    case ValueType::POINTER:
            //        return ByteVector::pack(this->get<void*>());

        default:
            return fallback;
        }
    }

    dt::TimePoint Value::as_timepoint(const dt::TimePoint &fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::NONE:
            return fallback;

        case ValueType::BOOL:
            return fallback;

        case ValueType::TIMEPOINT:
            return this->get<dt::TimePoint>();

        case ValueType::STRING:
            return dt::js_to_timepoint(this->get<std::string>(), fallback);

        case ValueType::BYTEVECTOR:
            try
            {
                return this->get<ByteVector>().unpack<dt::TimePoint>();
            }
            catch (const std::out_of_range &)
            {
                return fallback;
            }

        default:
            return dt::TimePoint(this->as_duration(fallback.time_since_epoch()));
        }
    }

    dt::Duration Value::as_duration(const dt::Duration &fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::NONE:
            return fallback;

        case ValueType::REAL:
        case ValueType::COMPLEX:
            return dt::to_duration(this->as_real());

        case ValueType::UINT:
        case ValueType::SINT:
            return dt::to_duration(this->numeric_cast<time_t>());

        case ValueType::STRING:
            return dt::to_duration(this->get<std::string>());

        case ValueType::BYTEVECTOR:
            try
            {
                return this->get<ByteVector>().unpack<dt::Duration>();
            }
            catch (const std::out_of_range &)
            {
                return fallback;
            }

        case ValueType::TIMEPOINT:
            return this->get<dt::TimePoint>().time_since_epoch();

        case ValueType::DURATION:
            return this->get<dt::Duration>();

        default:
            return fallback;
        }
    }

    ValueList Value::as_valuelist() const noexcept
    {
        return this->as_valuelist({});
    }

    ValueList Value::as_valuelist(const ValueList &fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return *this->get<ValueListPtr>();

        case ValueType::KVMAP:
            return this->get<KeyValueMapPtr>()->values();

        case ValueType::TVLIST:
            return this->get<TaggedValueListPtr>()->values();

        case ValueType::COMPLEX:
            return {this->as_real(), this->as_imag()};

        default:
            return fallback;
        }
    }

    KeyValueMap Value::as_kvmap() const noexcept
    {
        return this->as_kvmap({});
    }

    KeyValueMap Value::as_kvmap(const KeyValueMap &fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::KVMAP:
            return *this->get<KeyValueMapPtr>();

        case ValueType::TVLIST:
            return this->get<TaggedValueListPtr>()->as_kvmap();

        case ValueType::COMPLEX:
            return {
                {REAL_PART, this->as_real()},
                {IMAG_PART, this->as_imag()},
            };

        default:
            return fallback;
        }
    }

    TaggedValueList Value::as_tvlist() const noexcept
    {
        return this->as_tvlist({});
    }

    TaggedValueList Value::as_tvlist(const TaggedValueList &fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return this->get<ValueListPtr>()->as_tvlist();

        case ValueType::KVMAP:
            return this->get<KeyValueMapPtr>()->as_tvlist();

        case ValueType::TVLIST:
            return *this->get<TaggedValueListPtr>();

        case ValueType::COMPLEX:
            return {
                {REAL_PART, this->as_real()},
                {IMAG_PART, this->as_imag()},
            };

        default:
            return fallback;
        }
    }

    ValueListPtr Value::get_valuelist() const noexcept
    {
        if (auto *ptr = this->get_if<ValueListPtr>())
        {
            return *ptr;
        }
        else
        {
            return {};
        }
    }

    KeyValueMapPtr Value::get_kvmap() const noexcept
    {
        if (auto *ptr = this->get_if<KeyValueMapPtr>())
        {
            return *ptr;
        }
        else
        {
            return {};
        }
    }

    TaggedValueListPtr Value::get_tvlist() const noexcept
    {
        if (auto *ptr = this->get_if<TaggedValueListPtr>())
        {
            return *ptr;
        }
        else
        {
            return {};
        }
    }

    Value &Value::operator[](const std::string &key)
    {
        switch (this->type())
        {
        case ValueType::KVMAP:
            return std::get<KeyValueMapPtr>(*this)->operator[](key);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->operator[](key);

        default:
            throw std::invalid_argument("Value instance is not mappable");
        }
    }

    Value &Value::operator[](const uint index)
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

    const Value &Value::front(const Value &fallback) const noexcept
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

    const Value &Value::back(const Value &fallback) const noexcept
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

    const Value &Value::get(const std::string &key, const Value &fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::KVMAP:
            return std::get<KeyValueMapPtr>(*this)->get(key, fallback);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->get(key, fallback);

        default:
            return fallback;
        }
    }

    const Value &Value::get(const uint index, const Value &fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return std::get<ValueListPtr>(*this)->get(index, fallback);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->get(index, fallback);

        default:
            return fallback;
        }
    }

    const Value &Value::get(const int index, const Value &fallback) const noexcept
    {
        switch (this->type())
        {
        case ValueType::VALUELIST:
            return std::get<ValueListPtr>(*this)->get(index, fallback);

        case ValueType::TVLIST:
            return std::get<TaggedValueListPtr>(*this)->get(index, fallback);

        default:
            return fallback;
        }
    }

    void Value::to_stream(std::ostream &stream) const
    {
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
            stream << this->as_real();
            break;

        case ValueType::COMPLEX:
            stream << this->as_complex();
            break;

        case ValueType::STRING:
            stream << this->get<std::string>();
            break;

        case ValueType::BYTEVECTOR:
            stream << this->get<ByteVector>();
            break;

        case ValueType::TIMEPOINT:
            stream << this->get<dt::TimePoint>();
            break;

        case ValueType::DURATION:
            stream << this->get<dt::Duration>();
            break;

        case ValueType::VALUELIST:
            this->get_valuelist()->to_stream(stream);
            break;

        case ValueType::KVMAP:
            this->get_kvmap()->to_stream(stream);
            break;

        case ValueType::TVLIST:
            this->get_tvlist()->to_stream(stream);
            break;

        default:
            stream << "(Unknown value type " << this->type() << ")";
            break;
        }
    }

    void Value::to_literal_stream(std::ostream &stream) const
    {
        switch (this->type())
        {
        case ValueType::NONE:
            stream << "null";
            break;

        case ValueType::BYTEVECTOR:
            stream << "%" << this->get<ByteVector>().to_base64();
            break;

        case ValueType::TIMEPOINT:
            stream << std::quoted(dt::to_js_string(this->get<dt::TimePoint>()));
            break;

        case ValueType::STRING:
            stream << std::quoted(this->get<std::string>());
            break;

        default:
            this->to_stream(stream);
            break;
        }
    }

    Value Value::from_literal(const std::string &literal)
    {
        switch (Value::literal_type(literal))
        {
        case ValueType::NONE:
            return nullvalue;

        case ValueType::BOOL:
            return str::convert_to<bool>(literal, false);

        case ValueType::CHAR:
            return literal[1];

        case ValueType::UINT:
            return str::convert_to<largest_uint>(literal, 0);

        case ValueType::SINT:
            return str::convert_to<largest_sint>(literal, 0);

        case ValueType::REAL:
            return str::convert_to<double>(literal, 0.0);

        case ValueType::COMPLEX:
            return str::convert_to<double>(literal, 0.0);

        case ValueType::STRING:
            return str::unquoted(literal);

        case ValueType::BYTEVECTOR:
            return ByteVector::from_base64(literal.substr(1));

        case ValueType::TIMEPOINT:
            return dt::to_timepoint(literal);

        case ValueType::DURATION:
            return dt::to_duration(literal);

        default:
            return nullvalue;
        }
    }

    ValueType Value::literal_type(const std::string &literal)
    {
        static const std::vector<std::pair<ValueType, std::regex>> rxlist = {
            {ValueType::NONE, std::regex("^(null|NULL|nullptr)?$")},
            {ValueType::BOOL, std::regex("^(false|true)$", std::regex::icase)},
            {ValueType::CHAR, std::regex("^'.'$")},
            {ValueType::SINT, std::regex("^[+-][[:digit:]]+$")},
            {ValueType::UINT, std::regex("^([[:digit:]]+|0x[[:xdigit:]]+)$", std::regex::icase)},
            {ValueType::REAL,
             std::regex("^[+-]?[[:digit:]]+(\\.[[:digit:]]*)?([eE][+-]?[[:digit:]]+)?$")},
            {ValueType::BYTEVECTOR, std::regex("^%[[:alnum:]\\+/]+={0,2}$")},
            {ValueType::STRING, std::regex("\"((?:\\\\.|[^\"\\\\\\r\\n])*)\"")},
            // {ValueType::STRING, std::regex("\"((?>\\\\\\\\|\\\\\"|[^\"\\r\\n])*)\"")},
            {ValueType::TIMEPOINT,
             std::regex("^(\\d{4}-\\d{2}-\\d{2})([@T\\s])"
                        "(\\d{2}:\\d{2}:\\d{2})(?:\\.(\\d+))?"
                        "(?:\\s?(\\w+|[+-]\\d{2,4}))?$")},
            {ValueType::DURATION, std::regex("(\\d{2}:\\d{2}:\\d{2})(?:\\.(\\d+))?$")},
        };

        for (const auto &[candidate, rx] : rxlist)
        {
            if (std::regex_match(literal.begin(), literal.end(), rx))
            {
                return candidate;
            }
        }

        return ValueType::STRING;
    }

    ValueType Value::implicit_type(const ValueBase &base) noexcept
    {
        return base.valueless_by_exception()
                   ? ValueType::NONE
                   : static_cast<ValueType>(base.index());
    }

    const Value emptyvalue;

}  // namespace core::types
