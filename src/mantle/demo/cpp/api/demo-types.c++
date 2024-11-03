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

    bool Greeting::operator==(const Greeting &other) const
    {
        return ((this->text == other.text) &&
                (this->identity == other.identity) &&
                (this->implementation == other.implementation) &&
                (this->birth == other.birth) &&
                (this->data == other.data));
    }

    bool Greeting::operator!=(const Greeting &other) const
    {
        return !(*this == other);
    }

    void Greeting::to_stream(std::ostream &stream) const
    {
        core::types::PartsList parts;
        parts.add_string("text", this->text);
        parts.add_string("identity", this->identity);
        parts.add_string("implementation", this->implementation);
        parts.add("birth", this->birth, this->birth != core::dt::TimePoint());
        parts.add("uptime", core::dt::Clock::now() - this->birth);
        parts.add_value("data", this->data, !this->data.empty());
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

    bool TimeData::operator==(const TimeData &other) const
    {
        return ((this->timepoint == other.timepoint) &&
                (this->localtime == other.localtime) &&
                (this->utctime == other.utctime));
    }

    bool TimeData::operator!=(const TimeData &other) const
    {
        return !(*this == other);
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
