// -*- c++ -*-
//==============================================================================
/// @file demo-types.c++
/// @brief Example App - common data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

// Application specific modules
#include "demo-types.h++"

// Shared modules
#include "types/value.h++"
#include "types/partslist.h++"

namespace demo
{
    //==========================================================================
    /// @class Greeting

    Greeting::Greeting(const std::string &text,
                       const std::string &identity,
                       const std::string &implementation,
                       const core::dt::TimePoint &birth,
                       const core::types::KeyValueMap &data)
        : text(text),
          identity(identity),
          implementation(implementation),
          birth(birth),
          data(data)
    {
    }

    void Greeting::to_stream(std::ostream &stream) const
    {
        core::types::PartsList parts;
        parts.add("text", this->text);
        parts.add("identity", this->identity);
        parts.add("implementation", this->implementation, !this->implementation.empty());
        parts.add("birth", this->birth, this->birth != core::dt::TimePoint());
        parts.add("uptime", core::dt::Clock::now() - this->birth);
        parts.add("data", this->data, !this->data.empty());
        stream << parts;
    }

    //==========================================================================
    /// @class TimeData
    /// @brief Time representation from the server.

    TimeData::TimeData(const core::dt::TimePoint &tp)
        : timepoint(tp),
          localtime(core::dt::localtime(tp)),
          utctime(core::dt::gmtime(tp))
    {
    }

    TimeData::TimeData(const core::dt::TimePoint &tp,
                       const std::tm &localtime,
                       const std::tm &utctime)
        : timepoint(tp),
          localtime(localtime),
          utctime(utctime)
    {
    }

    void TimeData::to_stream(std::ostream &stream) const
    {
        core::str::format(stream,
                        "TimeData(epoch=%d, local=%T, utc=%Z)",
                        core::dt::to_time_t(this->timepoint),
                        this->localtime,
                        this->utctime);
    }

}  // namespace demo
