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

    protected:
        using Super::Super;

    public:
        void to_stream(std::ostream &stream) const override;

    public:
        const Value &front(const Value &fallback = {}) const noexcept;
        const Value &back(const Value &fallback = {}) const noexcept;
        const Value &get(uint index, const Value &fallback = {}) const noexcept;
        const Value &get(int index, const Value &fallback = {}) const noexcept;
        TaggedValueList as_tvlist() const noexcept;

        template <class T>
        std::vector<T> filter_by_type() const
        {
            std::vector<T> result;
            result.reserve(this->size());
            for (const types::Value &value: *this)
            {
                if (const T *ptr = value.get_if<T>())
                {
                    result.push_back(*ptr);
                }
            }
            return result;
        }
    };

}  // namespace core::types
