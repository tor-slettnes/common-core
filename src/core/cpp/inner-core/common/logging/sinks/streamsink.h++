/// -*- c++ -*-
//==============================================================================
/// @file streamsink.h++
/// @brief Log sink for stream-based backend, (stdout/stderr, file, ...)
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "messagesink.h++"
#include "types/create-shared.h++"
#include "status/level.h++"
#include "types/value.h++"
#include "types/valuemap.h++"

#include <ostream>
#include <string>

namespace core::logging
{
    //==========================================================================
    /// \class StreamSink
    /// \brief Abstract base for logging to output streams (stdout, stderr, file)

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
        /// \brief Constructor
        /// \param[in] stream
        ///     Stream to log the messages to
        StreamSink(std::ostream &stream);
        StreamSink(std::ostream &&stream);

    public:
        void capture_message(const Message::ptr &msg) override;

    private:
        void load_styles(const types::KeyValueMap &stylemap);
        void load_styles(const types::KeyValueMap &stylemap,
                         MessagePart part,
                         const std::string &partname);

    protected:
        std::ostream &stream;
        types::ValueMap<status::Level, StyleMap> styles;
    };
}  // namespace core::logging
