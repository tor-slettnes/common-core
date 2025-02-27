/// -*- c++ -*-
//==============================================================================
/// @file streamsink.h++
/// @brief Log sink for stream-based backend, (stdout/stderr, file, ...)
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "messagesink.h++"
#include "factory.h++"
#include "status/level.h++"
#include "types/value.h++"
#include "types/valuemap.h++"
#include "types/create-shared.h++"

#include <ostream>
#include <string>
#include <functional>
#include <iostream>

namespace core::logging
{
    //--------------------------------------------------------------------------
    /// @class StreamSink
    /// @brief Abstract base for logging to output streams (stdout, stderr, file)

    class StreamSink : public MessageSink,
                       public types::enable_create_shared<StreamSink>
    {
        enum MessagePart
        {
            INTRO,
            TEXT,
            RESET
        };

        using StyleMap = types::ValueMap<MessagePart, std::string>;
        using Super = MessageSink;

    protected:
        /// @brief Constructor
        /// @param[in] sink_id
        ///     Identity of this log sink
        /// @param[in] stream
        ///     Stream to log the messages to
        StreamSink(const std::string &sink_id,
                   std::ostream &stream);

        StreamSink(const std::string &sink_id,
                   std::ostream &&stream);

    public:
        void load_settings(const types::KeyValueMap &settings) override;
        bool handle_message(const Message::ptr &message) override;

    private:
        void load_styles(const types::KeyValueMap &stylemap);
        void load_styles(const types::KeyValueMap &stylemap,
                         MessagePart part,
                         const std::string &partname);

    protected:
        std::ostream &stream;
        types::ValueMap<status::Level, StyleMap> styles;
    };


    //--------------------------------------------------------------------------
    // Add factory instances to create `stdout` and `stderr` log options

    inline static SinkFactory stdout_factory(
        "stdout",
        "Log to standard output.",
        [](const SinkID &sink_id) -> Sink::ptr {
            return StreamSink::create_shared(sink_id, std::cout);
        });

    inline static SinkFactory stderr_factory(
        "stderr",
        "Log to standard error. Enabled by default if standard output is a terminal.",
        [](const SinkID &sink_id) -> Sink::ptr {
            return StreamSink::create_shared(sink_id, std::cerr);
        },
        DefaultOption::IF_INTERACTIVE);
}  // namespace core::logging
