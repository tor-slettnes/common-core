/// -*- c++ -*-
//==============================================================================
/// @file logfilesink.c++
/// @brief Log to file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logfilesink.h++"

#include "chrono/date-time.h++"
#include "platform/path.h++"

#include <iomanip>

namespace shared::logging
{
    LogFileSink::LogFileSink(const std::string &path_template,
                             const dt::Duration &rotation_interval)
        : Super(),
          RotatingPath(path_template, ".log", rotation_interval)
    {
    }

    void LogFileSink::open()
    {
        this->open(dt::Clock::now());
    }

    void LogFileSink::open(const dt::TimePoint &tp)
    {
        // std::lock_guard(this->mtx_);
        this->update_current_path(tp);
        fs::create_directories(this->current_path().parent_path());
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

    void LogFileSink::rotate(const dt::TimePoint &tp)
    {
        this->close();
        this->open(tp);
    }

    void LogFileSink::capture_message(const Message::Ref &msg)
    {
        if (this->stream_ && this->stream_->good())
        {
            this->check_rotation(msg->timepoint());

            // auto lck = std::lock_guard(this->mtx_);
            auto &stream = *this->stream_;

            dt::tp_to_stream(stream, msg->timepoint(), true, 3, "%F|%T");

            stream << "|"
                   << std::setfill(' ')
                   << std::setw(8)
                   << msg->level()
                   << std::setw(0)
                   << "|";

            this->send_preamble(stream, msg);
            stream << msg->text() << std::endl;
        }
    }

}  // namespace shared::logging
