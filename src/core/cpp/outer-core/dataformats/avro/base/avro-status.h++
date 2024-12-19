/// -*- c++ -*-
//==============================================================================
/// @file avro-status.c++
/// @brief Check return status from Avro calls; throw if error
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "string/misc.h++"

#include <avro.h>


#include <string>
#include <stdexcept>

namespace core::avro
{
    inline std::runtime_error error(const std::string &context)
    {
        return std::runtime_error(
            core::str::join({context, avro_strerror()}, ": "));
    }

    inline int checkstatus(int status, const std::string &context = {})
    {
        if (status != 0)
        {
            throw error(context);
        }
        return status;
    }

    template <class PT>
    static PT *checkstatus(PT *pointer, const std::string &context = {})
    {
        if (pointer == nullptr)
        {
            throw error(context);
        }
        return pointer;
    }
} // namespace core::avro
