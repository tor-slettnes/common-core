// -*- c++ -*-
//==============================================================================
/// @file python-exception.h++
/// @brief Python error
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "python-containerobject.h++"
#include "status/exception.h++"
#include "types/value.h++"

namespace cc::python
{
    class Exception : public cc::exception::Exception<std::runtime_error>
    {
        using This = Exception;
        using Super = cc::exception::Exception<std::runtime_error>;

    public:
        // Exception(PyObject *exc, bool borrowed = false);
        Exception(const std::string &text,
                  const std::string &symbol = {},
                  const types::KeyValueMap &attributes = {});
        std::string text() const noexcept override;
        types::ValueList args() const noexcept;
    };
}  // namespace cc::python
