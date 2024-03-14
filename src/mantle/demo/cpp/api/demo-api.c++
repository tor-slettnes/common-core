// -*- c++ -*-
//==============================================================================
/// @file demo-api.c++
/// @brief Demo app - Abstract C++ interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

// Application specific modules
#include "demo-api.h++"

// Shared modules
#include "string/format.h++"

namespace demo
{
    //==========================================================================
    /// @class API

    API::API(
        const std::string &identity,
        const std::string &implementation,
        const core::dt::TimePoint &birth)
        : identity_(identity),
          implementation_(implementation),
          birth_(birth)
    {
    }

    void API::say_hello(const std::string &message,
                        const core::types::KeyValueMap &data)
    {
        this->say_hello(Greeting(message,
                                 this->identity(),
                                 this->implementation(),
                                 this->birth(),
                                 data));
    }

    std::string API::identity() const
    {
        return this->identity_;
    }

    std::string API::implementation() const
    {
        return this->implementation_;
    }

    core::dt::TimePoint API::birth() const
    {
        return this->birth_;
    }
}  // namespace demo
