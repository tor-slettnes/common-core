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
                       const shared::dt::TimePoint &birth,
                       const shared::types::KeyValueMap &data)
        : text(text),
          identity(identity),
          implementation(implementation),
          birth(birth),
          data(data)
    {
    }

    void Greeting::to_stream(std::ostream &stream) const
    {
        shared::types::PartsList parts;
        parts.add("text", this->text);
        parts.add("identity", this->identity);
        parts.add("implementation", this->implementation, !this->implementation.empty());
        parts.add("birth", this->birth, this->birth != shared::dt::TimePoint());
        parts.add("uptime", shared::dt::Clock::now() - this->birth);
        parts.add("data", this->data, !this->data.empty());
        stream << parts;
    }

    //==========================================================================
    /// @class TimeData
    /// @brief Time representation from the server.

    TimeData::TimeData(const shared::dt::TimePoint &tp)
        : timepoint(tp),
          localtime(shared::dt::localtime(tp)),
          utctime(shared::dt::gmtime(tp))
    {
    }

    TimeData::TimeData(const shared::dt::TimePoint &tp,
                       const std::tm &localtime,
                       const std::tm &utctime)
        : timepoint(tp),
          localtime(localtime),
          utctime(utctime)
    {
    }

    void TimeData::to_stream(std::ostream &stream) const
    {
        shared::str::format(stream,
                        "TimeData(epoch=%s, local=%s, utc=%s)",
                        shared::dt::to_time_t(this->timepoint),
                        shared::dt::to_string(this->localtime),
                        shared::dt::to_string(this->utctime));
    }

}  // namespace demo
