/// -*- c++ -*-
//==============================================================================
/// @file protobuf-quantities.c++
/// @brief Encode/decode routines for ProtoBuf quantity types
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-quantities.h++"

namespace protobuf
{
    void encode_quantity(double value,
                         int decimal_exponent,
                         cc::protobuf::quantities::Scalar *scalar)
    {
        scalar->set_value(value);
        scalar->set_decimal_exponent(decimal_exponent);
    }

    void encode_quantity(double value,
                         cc::protobuf::quantities::Scalar *scalar)
    {
        scalar->set_value(value);
    }
} // namespace protobuf
