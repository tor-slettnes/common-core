/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-demo.h++
/// @brief Encode/decode routines for Common Core Demo types
/// @author Tor Slettnes
//==============================================================================

#include "demo-types.hpp"  // Generated from `demo-types.idl`
#include "demo-types.h++"  // Native types
#include "translate-idl-common.h++"
#include "translate-idl-variant.h++"

namespace idl
{
    //==========================================================================
    // Greeting
    void encode(const demo::Greeting &native,
                CC::Demo::Greeting *idl);

    void decode(const CC::Demo::Greeting &idl,
                demo::Greeting *native);

    //==========================================================================
    // TimeData
    void encode(const demo::TimeData &native,
                CC::Demo::TimeData *idl);

    void decode(const CC::Demo::TimeData &idl,
                demo::TimeData *native);

    //==========================================================================
    // TimeStruct
    void encode(const std::tm &native,
                CC::Demo::TimeStruct *idl);

    void decode(const CC::Demo::TimeStruct &idl,
                std::tm *native);

}  // namespace idl
