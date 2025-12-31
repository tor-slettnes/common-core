// -*- c++ -*-
//==============================================================================
/// @file python-exception.c++
/// @brief Python error
/// @author Tor Slettnes
//==============================================================================

#include "python-exception.h++"
#include "python-containerobject.h++"
#include "platform/path.h++"

namespace core::python
{
    // Exception::Exception(PyObject *exc, bool borrowed)
    //     : ContainerObject(exc, borrowed),
    //       Super(
    //           status::Error(
    //               ContainerObject::to_string(),  // text
    //               status::Domain::APPLICATION,   // domain
    //               platform::path->exec_name(),   // origin
    //               0,                             // code
    //               this->type_name(),             // symbol
    //               status::Level::ERROR,         // level
    //               status::Flow::CANCELLED,       // flow
    //               {}                             // timepoint
    //               // ContainerObject(exc, true).attributes_as_kvmap()))
    //               ))
    // {
    // }

    Exception::Exception(const std::string &text,
                         const std::string &symbol,
                         const types::KeyValueMap &attributes)
        : Super(
              status::Error(
                  text,                         // text
                  status::Domain::APPLICATION,  // domain
                  platform::path->exec_name(),  // origin
                  0,                            // code
                  symbol,                       // symbol
                  status::Level::ERROR,        // level
                  {},                           // timepoint
                  attributes))                  // attributes
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

}  // namespace core::python
