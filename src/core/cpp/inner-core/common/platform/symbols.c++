/// -*- c++ -*-
//==============================================================================
/// @file symbols.c++
/// @brief Internal symbols - abstract provider
/// @author Tor Slettnes
//==============================================================================

#include "symbols.h++"
#include "string/format.h++"

#include <string.h>

#include <random>
#include <sstream>

namespace cc::core::platform
{
    // Poor man's UUID generator.
    // Overridden for OSes that provide their own implementations,
    // e.g. `uuid_generate()` on Linux.
    types::UUID SymbolsProvider::uuid() const noexcept
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0x00, 0xFF);
        static std::uniform_int_distribution<> dis6(0x40, 0x4F);
        static std::uniform_int_distribution<> dis8(0x80, 0xBF);

        types::UUID uuid;
        for (std::size_t i = 0; i < core::types::UUID_SIZE; i++)
        {
            switch (i)
            {
            case 6:
                // High four bits of byte 6 represent version;
                // 4 means randomly generated
                uuid[i] = dis6(gen);
                break;

            case 8:
                // High four bits of byte 8 represent UUID type;
                // 8..B means type 1 (RFC 4122/DCE 1.1)
                uuid[i] = dis8(gen);
                break;

            default:
                uuid[i] = dis(gen);
                break;
            }
        }
        return uuid;
    }

    std::string SymbolsProvider::uuid_string() const noexcept
    {
        return this->uuid().to_string();
    }

    std::string SymbolsProvider::errno_name(int num) const noexcept
    {
        return "";
    }

    std::string SymbolsProvider::errno_string(int num) const noexcept
    {
        return ::strerror(num);
    }

    ProviderProxy<SymbolsProvider> symbols("symbols");

}  // namespace cc::core::platform
