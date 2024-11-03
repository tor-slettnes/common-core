/// -*- c++ -*-
//==============================================================================
/// @file logfilesink.c++
/// @brief Log to file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logfilesink.h++"
#include "platform/path.h++"
#include <iomanip>

namespace core::logging
{
    LogFileSink::LogFileSink(const std::string &sink_id,
                             status::Level threshold,
                             const std::string &path_template,
                             const dt::DateTimeInterval &rotation_interval,
                             bool local_time)
        : AsyncLogSink(sink_id, threshold),
          RotatingPath(sink_id, path_template, ".log", rotation_interval, local_time)
    {
    }

    void LogFileSink::open()
    {
        this->open_file(dt::Clock::now());
        AsyncLogSink::open();
    }

    void LogFileSink::close()
    {
        AsyncLogSink::close();
        this->close_file();
    }

    void LogFileSink::open_file(const dt::TimePoint &tp)
    {
        RotatingPath::open_file(tp);
        this->stream_ = std::make_shared<std::ofstream>(this->current_path(), std::ios::ate);
        this->stream_->exceptions(std::ios::failbit);
    }

    void LogFileSink::close_file()
    {
        if (this->stream_)
        {
            this->stream_->close();
            this->stream_.reset();
        }
        RotatingPath::close_file();
    }

    void LogFileSink::capture_event(const status::Event::ptr &event)
    {
        if (this->stream_ && this->stream_->good())
        {
            this->check_rotation(event->timepoint());
            auto &stream = *this->stream_;
            this->send_preamble(stream, event);
            stream << event->text() << std::endl;
        }
    }

}  // namespace core::logging
