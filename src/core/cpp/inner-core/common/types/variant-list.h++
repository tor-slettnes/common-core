/// -*- c++ -*-
//==============================================================================
/// @file variant-list.h++
/// @brief Vector of `Value` instances
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "variant-value.h++"
#include "variant-tvlist.h++"
#include "streamable.h++"
#include "types/create-shared.h++"

namespace core::types
{
    class ValueList : public std::deque<Value>,
                      public Streamable,
                      public enable_create_shared<ValueList>
    {
        using Super = std::deque<Value>;
        using AppendResult = std::pair<ValueList::iterator, bool>;

    public:
        using ptr = std::shared_ptr<ValueList>;

    protected:
        using Super::Super;

    public:
        // No-ops for compatibility with std::vector<>;
        void reserve(std::size_t capacity) {}
        void shrink_to_fit() {}

    public:
        const Value &front(const Value &fallback = {}) const noexcept;
        const Value &back(const Value &fallback = {}) const noexcept;
        const Value &get(uint index, const Value &fallback = {}) const noexcept;
        const Value &get(int index, const Value &fallback = {}) const noexcept;
        TaggedValueList as_tvlist() const noexcept;

        /// Return a copy where items with empty values are filtered out
        ValueList filtered_values() const noexcept;

        iterator append(const Value &value);
        iterator append(Value &&value);

        AppendResult append_if(bool condition, const Value &value);
        AppendResult append_if(bool condition, Value &&value);

        AppendResult append_if_value(const Value &value);
        AppendResult append_if_value(Value &&value);

    public:
        void to_stream(std::ostream &stream) const override;

    public:
        template <class T>
        std::optional<T> try_get_as(const uint index) const
        {
            return this->get(index).try_convert_to<T>();
        }

        template <class T>
        std::vector<T> filter_by_type() const
        {
            std::vector<T> result;
            result.reserve(this->size());
            for (const types::Value &value : *this)
            {
                if (const T *ptr = value.get_if<T>())
                {
                    result.push_back(*ptr);
                }
            }
            return result;
        }

        template<class T>
        static std::shared_ptr<ValueList> create_shared_from(const T &obj)
        {
            auto vlist = std::make_shared<ValueList>();
            (*vlist) << obj;
            return vlist;
        }

        template<class T>
        static ValueList create_from(const T &obj)
        {
            ValueList vlist;
            vlist << obj;
            return vlist;
        }
    };

    //--------------------------------------------------------------------------
    // Non-member functions

    template <class T>
    ValueList &operator<<(ValueList &vlist, const T &inputs)
    {
        vlist.reserve(vlist.size() + inputs.size());
        for (const auto &input: inputs)
        {
            vlist.emplace_back() << input;
        }
        return vlist;
    }


}  // namespace core::types
