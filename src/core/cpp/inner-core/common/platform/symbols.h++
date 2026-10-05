/// -*- c++ -*-
//==============================================================================
/// @file symbols.h++
/// @brief Internal symbols - abstract provider
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "provider.h++"
#include "types/uuid.h++"

#define TYPE_NAME_FULL(entity) ::cc::core::platform::symbols->cpp_demangle(typeid(entity).name(), false)
#define TYPE_NAME_BASE(entity) ::cc::core::platform::symbols->cpp_demangle(typeid(entity).name(), true)

/// Default filesystem paths.
namespace cc::core::platform
{
    /// @brief Abstract provider for path-related functions
    class SymbolsProvider : public Provider
    {
        using Super = Provider;

    public:
        // Inherit constructor
        using Super::Super;

        /// @brief Generate a Universally Unique IDentifier
        virtual types::UUID uuid() const noexcept;
        virtual std::string uuid_string() const noexcept;

        virtual std::string errno_name(int num) const noexcept;
        virtual std::string errno_string(int num) const noexcept;

        /// @brief Demangle a type/class name, i.e. `typeid(Class).name`
        virtual std::string cpp_demangle(
            const std::string &abiname,
            bool stem_only) const noexcept = 0;
    };

    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<SymbolsProvider> symbols;

}  // namespace cc::core::platform
