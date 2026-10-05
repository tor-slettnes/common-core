// -*- c++ -*-
//==============================================================================
/// @file test-variant.c++
/// @brief C++ core - test routines
/// @author Tor Slettnes
//==============================================================================

#include "types/bytevector.h++"

#include <gtest/gtest.h>

namespace cc::core::types
{
    TEST(ByteVector, ConstructFromHex)
    {
        std::string hexstring = "0123456789-abcdef";
        ByteVector bv1 = ByteVector::from_hex(hexstring, {'-'});
        ByteVector bv2 = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
        EXPECT_EQ(bv1, bv2);
    }
}  // namespace cc::core::types
