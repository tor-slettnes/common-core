/// -*- c++ -*-
//==============================================================================
/// @file variant-list.h++
/// @brief Vector of `Value` instances
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "variant-value.h++"
#include "variant-tvlist.h++"
#include "streamable.h++"
#include "types/create-shared.h++"

namespace core::types
{
    class ValueList : public std::vector<Value>,
                      public Streamable,
                      public enable_create_shared<ValueList>
    {
        using Super = std::vector<Value>;
        using AppendResult = std::pair<ValueList::iterator, bool>;

    public:
        using ptr = std::shared_ptr<ValueList>;

    protected:
        using Super::Super;

    public:
        // No-ops for compatibility with std::vector<>;
        // void reserve(std::size_t capacity) {}
        // void shrink_to_fit() {}

    public:
        const Value &front(const Value &fallback = {}) const noexcept;
        const Value &back(const Value &fallback = {}) const noexcept;
        const Value &get(uint index, const Value &fallback = {}) const noexcept;
        const Value &get(int index, const Value &fallback = {}) const noexcept;

        std::optional<Value> try_get(uint index) const noexcept;
        std::optional<Value> try_get(int index) const noexcept;

        TaggedValueList as_tvlist() const noexcept;

        /// Return a copy where items with empty values are filtered out
        ValueList filtered_values() const noexcept;

        /// @brief
        ///    Add values from an existing ValueList instance
        /// @param[in] other
        ///    ValueList instance from which to import
        /// @return
        ///     A reference to this updated instance.
        ValueList &extend(const ValueList &other);
        ValueList &extend(ValueList &&other);


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
        void filter_into(std::vector<T> *vector) const
        {
            vector->reserve(vector->size() + this->size());
            for (const types::Value &value : *this)
            {
                if (const T *ptr = value.get_if<T>())
                {
                    vector->push_back(*ptr);
                }
            }
        }

        template <class T>
        std::vector<T> filter_by_type() const
        {
            std::vector<T> result;
            this->filter_into<T>(&result);
            return result;
        }

        template <class ContainerT>
        void populate_from(const ContainerT &container)
        {
            this->reserve(this->size() + container.size());
            for (const auto &item : container)
            {
                this->emplace_back(item);
            }
        }

        template <class ContainerT>
        static std::shared_ptr<ValueList> create_shared_from(const ContainerT &container)
        {
            auto list = std::make_shared<ValueList>();
            (*list) << container;
            return list;
        }

        template <class ContainerT>
        static ValueList create_from(const ContainerT &container)
        {
            ValueList list;
            list << container; 
            return list;
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
