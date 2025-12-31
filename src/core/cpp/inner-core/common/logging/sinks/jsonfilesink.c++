/// -*- c++ -*-
//==============================================================================
/// @file jsonfilesink.c++
/// @brief Log to JSON file
/// @author Tor Slettnes
//==============================================================================

#include "jsonfilesink.h++"
#include "platform/path.h++"

namespace core::logging
{
    JsonFileSink::JsonFileSink(const std::string &sink_id)
        : Super(sink_id),
          RotatingPath(sink_id, ".jsonlog")
    {
    }

    void JsonFileSink::load_settings(const types::KeyValueMap &settings)
    {
        Super::load_settings(settings);
        this->load_rotation(settings);
    }

    void JsonFileSink::open()
    {
        this->open_file(dt::Clock::now());
        Super::open();
    }

    void JsonFileSink::close()
    {
        Super::close();
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

    bool JsonFileSink::handle_item(const types::Loggable::ptr &item)
    {
        if (this->writer_)
        {
            this->check_rotation(item->timepoint());
            this->writer_->write(item->as_tvlist(),  // value
                                 false,              // pretty
                                 true);              // newline
            return true;
        }
        else
        {
            return false;
        }
    }
}  // namespace core::logging
