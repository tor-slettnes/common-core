/// -*- c++ -*-
//==============================================================================
/// @file getter.h++
/// @brief Abstract base for classes with blocking `get()` method
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <optional>

namespace core::types
{
    template <class T>
    class Getter
    {
    public:
        virtual ~Getter() {}

        virtual void close() = 0;
        virtual std::optional<T> get() = 0;

        T get(const T &fallback)
        {
            return this->get().value_or(fallback);
        }
    };
}  // namespace core::types
