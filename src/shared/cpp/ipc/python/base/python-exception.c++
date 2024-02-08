// -*- c++ -*-
//==============================================================================
/// @file python-exception.c++
/// @brief Python error
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "python-exception.h++"
#include "python-containerobject.h++"

namespace cc::python
{
    Exception::Exception(PyObject *exc, const std::string &module_name)
        : Super(
              status::Event(
                  "",                           // text
                  status::Domain::APPLICATION,  // domain
                  module_name,                  // origin
                  0,                            // code
                  SimpleObject(exc).name(),     // symbol
                  status::Level::FAILED,        // level
                  status::Flow::CANCELLED,      // flow
                  {},                           // timepoint
                  ContainerObject(exc).attributes_as_values()))
    {
    }

    Exception::Exception(const std::string &text, const std::string &module_name)
        : Super(
              status::Event(
                  text,                         // text
                  status::Domain::APPLICATION,  // domain
                  module_name,                  // origin
                  0,                            // code
                  "",                           // symbol
                  status::Level::FAILED,        // level
                  status::Flow::CANCELLED))
    {
    }

    std::string Exception::text() const noexcept
    {
        std::string text = Super::text();
        if (text.empty())
        {
            text = this->args().get(0).as_string();
        }
        return text;
    }

    types::ValueList Exception::args() const noexcept
    {
        return this->attribute("args").as_valuelist();
    }

}  // namespace cc::python
