/// -*- c++ -*-
//==============================================================================
/// @file logfilesink.h++
/// @brief Log to file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "messagesink.h++"
#include "rotatingpath.h++"
#include "types/create-shared.h++"
#include "types/filesystem.h++"

#include <fstream>

namespace core::logging
{
    class LogFileSink : public MessageSink,
                        public RotatingPath,
                        public types::enable_create_shared<LogFileSink>
    {
        using This = LogFileSink;
        using Super = MessageSink;

    protected:
        /// @brief Constructor
        /// @param[in] path_template
        ///     Path of the files where messaged will be logged to
        /// @param[in] rotation_interval
        ///     How often to switch to a new log file.
        LogFileSink(const std::string &path_template,
                    const dt::Duration &rotation_interval);

        void open() override;
        void open(const dt::TimePoint &tp);
        void close() override;
        void rotate(const dt::TimePoint &tp) override;
        void capture_message(const Message::Ref &msg) override;

    private:
        std::shared_ptr<std::ofstream> stream_;
    };
}  // namespace core::logging
