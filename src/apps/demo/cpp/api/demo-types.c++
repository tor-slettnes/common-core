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

namespace cc::demo
{
    //==========================================================================
    /// @class Greeting

    Greeting::Greeting(const std::string &text,
                       const std::string &identity,
                       const std::string &implementation,
                       const cc::dt::TimePoint &birth,
                       const cc::types::KeyValueMap &data)
        : text(text),
          identity(identity),
          implementation(implementation),
          birth(birth),
          data(data)
    {
    }

    void Greeting::to_stream(std::ostream &stream) const
    {
        cc::types::PartsList parts;
        parts.add("text", this->text);
        parts.add("identity", this->identity);
        parts.add("implementation", this->implementation, !this->implementation.empty());
        parts.add("birth", this->birth, this->birth != cc::dt::TimePoint());
        parts.add("uptime", cc::dt::Clock::now() - this->birth);
        parts.add("data", this->data, !this->data.empty());
        stream << parts;
    }

    //==========================================================================
    /// @class TimeData
    /// @brief Time representation from the server.

    TimeData::TimeData(const cc::dt::TimePoint &tp)
        : timepoint(tp),
          localtime(cc::dt::localtime(tp)),
          utctime(cc::dt::gmtime(tp))
    {
    }

    TimeData::TimeData(const cc::dt::TimePoint &tp,
                       const std::tm &localtime,
                       const std::tm &utctime)
        : timepoint(tp),
          localtime(localtime),
          utctime(utctime)
    {
    }

    void TimeData::to_stream(std::ostream &stream) const
    {
        cc::str::format(stream,
                        "TimeData(epoch=%s, local=%s, utc=%s)",
                        cc::dt::to_time_t(this->timepoint),
                        cc::dt::to_string(this->localtime),
                        cc::dt::to_string(this->utctime));
    }

}  // namespace cc::demo
