// -*- c++ -*-
//==============================================================================
/// @file demo-types.h++
/// @brief Example App - common data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

// Shared modules
#include "chrono/date-time.h++"
#include "types/streamable.h++"
#include "types/value.h++"
#include "logging/logging.h++"

// C++ STL modules
#include <ctime>
#include <functional>

namespace cc::demo
{
    // Classes defined below.
    class Greeting;
    class TimeData;

    //==========================================================================
    // Define a default log scope for this namespace. A specific logging
    // threshold may then be applied to log messages originating from here,
    // either in the application-specific configuration file, `defaults.json`,
    // or via the resulting command line option `--log-demo`.
    define_log_scope("demo");

    //==========================================================================
    /// @class Greeting
    /// @brief Data used to introduce ourselves to our peers
    class Greeting : public cc::types::Streamable
    {
    public:
        Greeting(const std::string &text = {},
                 const std::string &identity = {},
                 const std::string &implementation = {},
                 const cc::dt::TimePoint &birth = {},
                 const cc::types::KeyValueMap &data = {});

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        // Message presented by the greeter
        std::string text;

        // Our identity, by default the name of the executable
        std::string identity;

        // Implementation type ("CoreProvider", "StreamingExampleClient", etc).
        std::string implementation;

        // Timepoint when this process was launched
        cc::dt::TimePoint birth;

        // Arbitrary key/value pairs associated with the greeting
        cc::types::KeyValueMap data;
    };

    //==========================================================================
    /// @class TimeData
    /// @brief Time representation from the server.

    class TimeData : public cc::types::Streamable
    {
    public:
        TimeData(const cc::dt::TimePoint &tp = {});
        TimeData(const cc::dt::TimePoint &tp,
                 const std::tm &localtime,
                 const std::tm &utctime);

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        cc::dt::TimePoint timepoint;
        std::tm localtime;
        std::tm utctime;
    };

}  // namespace cc::demo
