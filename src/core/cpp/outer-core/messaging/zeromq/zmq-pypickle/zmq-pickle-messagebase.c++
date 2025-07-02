/// -*- c++ -*-
//==============================================================================
/// @file zmq-pickle-messagebase.c++
/// @brief Base for Python Pickle reader and writer
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-pickle-messagebase.h++"

namespace core::zmq
{
    PyPickleMessageBase::PyPickleMessageBase(
        const std::shared_ptr<Endpoint> &endpoint)
        : default_filter_(endpoint->setting("pickle filter"))
    {
    }

    Filter PyPickleMessageBase::default_filter()
    {
        return this->default_filter_;
    }

    Filter PyPickleMessageBase::pickle_filter(const types::Value &value)
    {
        if (const std::string *magic = value.get_if<std::string>())
        {
            return {magic->begin(), magic->end()};
        }
        else if (const types::ValueListPtr &list = value.get_valuelist_ptr())
        {
            auto numbers = list->filter_by_type<types::largest_uint>();
            return {numbers.begin(), numbers.end()};
        }
        else
        {
            return {};
        }
    }
}  // namespace core::zmq
