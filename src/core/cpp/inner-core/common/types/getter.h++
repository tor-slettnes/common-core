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
        virtual std::optional<T> get() = 0;

        template <class... Args>
        std::optional<T> get(const T &fallback)
        {
            return this->get().value_or(fallback);
        }
    };
}  // namespace core::tyepes
