// -*- c++ -*-
//==============================================================================
/// @file python-exception.h++
/// @brief Python error
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "status/exception.h++"
#include "types/value.h++"

namespace cc::python
{
    class Exception : cc::exception::Exception<std::runtime_error>
    {
        using This = Exception;
        using Super = cc::exception::Exception<std::runtime_error>;

    public:
        Exception(PyObject *exc, const std::string &module_name);
        Exception(const std::string &text, const std::string &module_name);
        std::string text() const noexcept override;
        types::ValueList args() const noexcept;
    };
}  // namespace cc::python