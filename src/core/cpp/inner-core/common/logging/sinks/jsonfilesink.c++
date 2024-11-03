/// -*- c++ -*-
//==============================================================================
/// @file jsonfilesink.c++
/// @brief Log to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "jsonfilesink.h++"
#include "platform/path.h++"

namespace core::logging
{
    JsonFileSink::JsonFileSink(const std::string &sink_id,
                               status::Level threshold,
                               const std::string &path_template,
                               const dt::DateTimeInterval &rotation_interval,
                               bool local_time)
        : AsyncLogSink(sink_id, threshold),
          RotatingPath(sink_id, path_template, ".json", rotation_interval, local_time)
    {
    }

    void JsonFileSink::open()
    {
        this->open_file(dt::Clock::now());
        AsyncLogSink::open();
    }

    void JsonFileSink::close()
    {
        AsyncLogSink::close();
        this->close_file();
    }

    void JsonFileSink::open_file(const dt::TimePoint &tp)
    {
        RotatingPath::open_file(tp);
        this->writer_ = std::make_shared<json::Writer>(this->current_path());
    }

    void JsonFileSink::close_file()
    {
        if (this->writer_)
        {
            this->writer_.reset();
        }
        RotatingPath::close_file();
    }

    void JsonFileSink::capture_event(const status::Event::ptr &event)
    {
        if (this->writer_)
        {
            this->check_rotation(event->timepoint());
            this->writer_->write(event->as_tvlist(), // value
                                 false,              // pretty
                                 true);              // newline
        }
    }
}  // namespace core::logging
