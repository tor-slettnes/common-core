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
    LogFileSink::LogFileSink(const std::string &path_template,
                             const dt::Duration &rotation_interval)
        : Super(),
          RotatingPath(path_template, ".log", rotation_interval)
    {
    }

    void LogFileSink::open(const dt::TimePoint &tp)
    {
        this->update_current_path(tp);
        this->stream_ = std::make_shared<std::ofstream>(this->current_path(), std::ios::ate);
        this->stream_->exceptions(std::ios::failbit);
    }

    void LogFileSink::close()
    {
        if (this->stream_)
        {
            this->stream_->close();
            this->stream_.reset();
        }
    }

    void LogFileSink::capture_event(const status::Event::ptr &event)
    {
        if (this->stream_ && this->stream_->good())
        {
            this->check_rotation(event->timepoint());

            // auto lck = std::lock_guard(this->mtx_);
            auto &stream = *this->stream_;

            dt::tp_to_stream(stream, event->timepoint(), true, 3, "%F|%T");

            stream << "|"
                   << std::setfill(' ')
                   << std::setw(8)
                   << event->level()
                   << std::setw(0)
                   << "|";

            this->send_preamble(stream, event);
            stream << event->text() << std::endl;
        }
    }

}  // namespace core::logging
