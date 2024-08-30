/// -*- c++ -*-
//==============================================================================
/// @file symbols.h++
/// @brief Internal symbols - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"

#define TYPE_NAME_FULL(entity) ::core::platform::symbols->cpp_demangle(typeid(entity).name(), false)
#define TYPE_NAME_BASE(entity) ::core::platform::symbols->cpp_demangle(typeid(entity).name(), true)

/// Default filesystem paths.
namespace core::platform
{
    /// @brief Abstract provider for path-related functions
    class SymbolsProvider : public Provider
    {
        using Super = Provider;

    public:
        // Inherit constructor
        using Super::Super;

        /// @brief Generate a Universally Unique IDentifier
        virtual std::string uuid() const noexcept = 0;

        virtual std::string errno_name(int num) const noexcept;
        virtual std::string errno_string(int num) const noexcept;

        /// @brief Demangle a type/class name, i.e. `typeid(Class).name`
        virtual std::string cpp_demangle(
            const std::string &abiname,
            bool stem_only) const noexcept = 0;
    };

    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<SymbolsProvider> symbols;

}  // namespace core::platform
