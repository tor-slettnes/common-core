/// -*- c++ -*-
//==============================================================================
/// @file protobuf-quantities.h++
/// @brief Encode/decode routines for ProtoBuf quantity types
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/protobuf/quantities/quantities.pb.h"  // generated from `variant.proto`

namespace cc::protobuf
{
    void encode_quantity(double value,
                         int decimal_exponent,
                         quantities::Scalar* scalar);

    void encode_quantity(double value,
                         quantities::Scalar* scalar);

    template <class Quantity, class Unit>
    void encode_quantity(double value,
                         int decimal_exponent,
                         Unit unit,
                         Quantity* quantity)
    {
        quantity->set_value(value);
        quantity->set_decimal_exponent(decimal_exponent);
        quantity->set_unit(unit);
    }

    template <class Quantity, class Unit>
    void encode_quantity(double value,
                         Unit unit,
                         Quantity* quantity)
    {
        quantity->set_value(value);
        quantity->set_unit(unit);
    }

}  // namespace cc::protobuf
