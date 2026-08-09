/// -*- c++ -*-
//==============================================================================
/// @file protobuf-quantities.c++
/// @brief Encode/decode routines for ProtoBuf quantity types
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-quantities.h++"

namespace cc::protobuf
{
    void encode_quantity(double value,
                         int decimal_exponent,
                         quantities::Scalar* scalar)
    {
        scalar->set_value(value);
        scalar->set_decimal_exponent(decimal_exponent);
    }

    void encode_quantity(double value,
                         quantities::Scalar* scalar)
    {
        scalar->set_value(value);
    }
}  // namespace cc::protobuf
