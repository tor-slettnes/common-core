/// -*- c++ -*-
//==============================================================================
/// @file jsonfilesink.c++
/// @brief Log to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "jsonfilesink.h++"
#include "platform/path.h++"

namespace shared::logging
{
    JsonFileSink::JsonFileSink(const std::string &path_template,
                               const dt::Duration &rotation_interval)
        : Super(),
          RotatingPath(path_template, ".json", rotation_interval)
    {
    }

    void JsonFileSink::open()
    {
        this->open(dt::Clock::now());
    }

    void JsonFileSink::open(const dt::TimePoint &tp)
    {
        this->update_current_path(tp);
        fs::create_directories(this->current_path().parent_path());
        this->writer_ = std::make_shared<json::JsonWriter>(this->current_path());
    }

    void JsonFileSink::close()
    {
        if (this->writer_)
        {
            this->writer_.reset();
        }
    }

    void JsonFileSink::rotate(const dt::TimePoint &tp)
    {
        this->close();
        this->open(tp);
    }

    void JsonFileSink::capture_event(const status::Event::Ref &event)
    {
        if (this->writer_)
        {
            this->check_rotation(event->timepoint());
            this->writer_->write(event->as_tvlist());
        }
    }
}  // namespace shared::logging
