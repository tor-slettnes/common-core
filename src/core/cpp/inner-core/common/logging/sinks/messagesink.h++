/// -*- c++ -*-
//==============================================================================
/// @file messagesink.h++
/// @brief Abstract base for sinks that log free-form text strings
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logsink.h++"
#include "../message/message.h++"

#include <string>
#include <memory>
#include <ostream>

namespace cc::logging
{
    //==========================================================================
    /// \class MessageSink
    /// \brief Abstract base for sinks that log free-form text strings

    class MessageSink : public LogSink
    {
        using Super = LogSink;

    public:
        using Ref = std::shared_ptr<MessageSink>;

    public:
        void set_include_context(bool include_context);
        bool include_context() const;

    protected:
        bool is_applicable(const types::Loggable &item) const override;
        void capture_event(const status::Event::Ref &event) override;
        virtual void capture_message(const Message::Ref &msg) = 0;

        void send_preamble(std::ostream &stream,
                           const Message::Ref &msg,
                           const std::string &suffix = ": ") const;

        std::string preamble(const Message::Ref &msg,
                             const std::string &suffix = "") const;

    protected:
    private:
        bool include_context_;
    };
}  // namespace cc::logging
