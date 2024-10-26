/// -*- c++ -*-
//==============================================================================
/// @file jsonfilesink.h++
/// @brief Log to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "asynclogsink.h++"
#include "rotatingpath.h++"
#include "json/writer.h++"
#include "types/filesystem.h++"
#include "types/create-shared.h++"

#include <filesystem>
#include <fstream>
#include <mutex>

namespace core::logging
{
    class JsonFileSink : public AsyncLogSink,
                         public RotatingPath,
                         public types::enable_create_shared<JsonFileSink>
    {
        using This = JsonFileSink;
        using Super = AsyncLogSink;

    protected:
        /// @brief Constructor
        /// @param[in] path_template
        ///     Path of the files where messaged will be logged to
        /// @param[in] rotation_interval
        ///     How often to switch to a new log file.
        JsonFileSink(const std::string &path_template,
                     const dt::Duration &rotation_interval);

        void open(const dt::TimePoint &tp) override;
        void close() override;
        void capture_event(const status::Event::ptr &event) override;

    private:
        std::shared_ptr<json::Writer> writer_;
    };
}  // namespace core::logging
