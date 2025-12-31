// -*- c++ -*-
//==============================================================================
/// @file demo-types.c++
/// @brief Example App - common data types
/// @author Tor Slettnes
//==============================================================================

// Application specific modules
#include "demo-types.h++"

// Shared modules
#include "types/value.h++"

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

    void Greeting::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->extend({
            {"text", this->text},
            {"identity", this->identity},
            {"implementation", this->implementation},
        });

        tvlist->append_if(
            this->birth != core::dt::epoch,
            "birth",
            this->birth);

        tvlist->append_if(
            this->birth != core::dt::epoch,
            "uptime",
            core::dt::Clock::now() - this->birth);

        tvlist->append_if(
            !this->data.empty(),
            "data",
            this->data);
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

    void TimeData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append("epoch", core::dt::to_time_t(this->timepoint));
        tvlist->append("local", core::str::format("%T", this->localtime));
        tvlist->append("utc", core::str::format("%Z", this->utctime));
    }

}  // namespace demo
