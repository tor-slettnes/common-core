// -*- c++ -*-
//==============================================================================
/// @file python-exception.c++
/// @brief Python error
/// @author Tor Slettnes
//==============================================================================

#include "python-exception.h++"
#include "python-containerobject.h++"
#include "platform/path.h++"

namespace cc::python
{
    // Exception::Exception(PyObject *exc, bool borrowed)
    //     : ContainerObject(exc, borrowed),
    //       Super(
    //           core::status::Error(
    //               ContainerObject::to_string(),  // text
    //               core::status::Domain::APPLICATION,   // domain
    //               core::platform::path->exec_name(),   // origin
    //               0,                             // code
    //               this->type_name(),             // symbol
    //               core::status::Level::ERROR,         // level
    //               core::status::Flow::CANCELLED,       // flow
    //               {}                             // timepoint
    //               // ContainerObject(exc, true).attributes_as_kvmap()))
    //               ))
    // {
    // }

    Exception::Exception(const std::string &text,
                         const std::string &symbol,
                         const core::types::KeyValueMap &attributes)
        : Super(
              core::status::Error(
                  text,                               // text
                  core::status::Domain::APPLICATION,  // domain
                  core::platform::path->exec_name(),  // origin
                  0,                                  // code
                  symbol,                             // symbol
                  core::status::Level::ERROR,         // level
                  {},                                 // timepoint
                  attributes))                        // attributes
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

    core::types::ValueList Exception::args() const noexcept
    {
        return this->attribute("args").as_valuelist();
    }

}  // namespace cc::python
