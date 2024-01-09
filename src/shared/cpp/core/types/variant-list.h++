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

namespace cc::types
{
    class ValueList : public std::vector<Value>,
                      public Streamable,
                      public enable_create_shared<ValueList>
    {
        using Super = std::vector<Value>;

    protected:
        using Super::Super;

    public:
        void to_stream(std::ostream &stream) const override;

    public:
        const Value &get(uint index, const Value &fallback = {}) const noexcept;
        const Value &get(int index, const Value &fallback = {}) const noexcept;
        TaggedValueList as_tvlist() const noexcept;
    };

}  // namespace cc::types
