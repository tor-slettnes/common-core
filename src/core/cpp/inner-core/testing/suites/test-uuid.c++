// -*- c++ -*-
//==============================================================================
/// @file test-variant.c++
/// @brief C++ core - test routines
/// @author Tor Slettnes
//==============================================================================

#include "types/uuid.h++"
#include "platform/symbols.h++"

#include <gtest/gtest.h>

namespace cc::core::types
{
    TEST(UUID, Unique)
    {
        UUID uuid1 = platform::symbols->uuid();
        UUID uuid2 = platform::symbols->uuid();
        EXPECT_NE(uuid1, uuid2);
    }

    TEST(UUID, ToString)
    {
        ByteVector bv{
            0x01,
            0x12,
            0x23,
            0x34,
            0x45,
            0x56,
            0x47,
            0x78,
            0x80,
            0x00,
            0x01,
            0x00,
            0xCD,
            0xDE,
            0xEF,
            0xF0,
        };

        UUID uuid{bv};
        EXPECT_EQ(uuid.to_string(), "01122334-4556-4778-8000-0100cddeeff0");
    }

    TEST(UUID, ConstructFromString)
    {
        std::string uuid_string{"12345678-9abc-4567-89ab-123456789abc"};
        EXPECT_EQ(UUID(uuid_string).to_string(), uuid_string);
    }

}  // namespace cc::core::types
