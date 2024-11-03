/// -*- c++ -*-
//==============================================================================
/// @file logfilesink.h++
/// @brief Log to file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "asynclogsink.h++"
#include "messageformatter.h++"
#include "rotatingpath.h++"
#include "types/create-shared.h++"
#include "types/filesystem.h++"

#include <fstream>

namespace core::logging
{
    class LogFileSink : public AsyncLogSink,
                        public MessageFormatter,
                        public RotatingPath,
                        public types::enable_create_shared<LogFileSink>
    {
        using This = LogFileSink;
        using Super = AsyncLogSink;

    protected:
        /// @brief Constructor
        /// @param[in] path_template
        ///     Path of the files where messaged will be logged to
        /// @param[in] rotation_interval
        ///     How often to switch to a new log file.
        LogFileSink(const std::string &sink_id,
                    status::Level threshold,
                    const std::string &path_template,
                    const dt::DateTimeInterval &rotation_interval,
                    bool local_time = true);

        void open() override;
        void close() override;
        void open_file(const dt::TimePoint &tp) override;
        void close_file() override;
        void capture_event(const status::Event::ptr &event) override;

    private:
        std::shared_ptr<std::ofstream> stream_;
    };
}  // namespace core::logging
