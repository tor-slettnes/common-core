/// -*- c++ -*-
//==============================================================================
/// @file messageformatter.h++
/// @brief Mix-in class to format messages for text-based log sinks
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "../message/message.h++"

#include <string>
#include <memory>
#include <ostream>

namespace core::logging
{
    //==========================================================================
    /// \class MessageFormatter
    /// \brief Mix-in class for traits specific to text-based log sinks

    class MessageFormatter
    {
    public:
        void set_include_context(bool include_context);
        bool include_context() const;

    protected:
        void send_preamble(std::ostream &stream,
                           const status::Event::ptr &event) const;

        std::string formatted(const status::Event::ptr &event) const;

    private:
        bool include_context_;
    };
}  // namespace core::logging
