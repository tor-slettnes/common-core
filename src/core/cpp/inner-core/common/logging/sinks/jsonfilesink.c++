/// -*- c++ -*-
//==============================================================================
/// @file jsonfilesink.c++
/// @brief Log to JSON file
/// @author Tor Slettnes
//==============================================================================

#include "jsonfilesink.h++"
#include "platform/path.h++"

namespace cc::core::logging
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
        this->load_keys(settings);
    }

    void JsonFileSink::load_keys(const types::KeyValueMap &settings)
    {
        if (auto keys = settings.get(SETTING_KEYS).get_valuelist_ptr())
        {
            this->set_keys(keys->filter_by_type<std::string>());
        }
    }

    const std::vector<std::string> &JsonFileSink::keys() const
    {
        return this->keys_;
    }

    void JsonFileSink::set_keys(std::vector<std::string> keys)
    {
        this->keys_ = std::move(keys);
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
        this->writer_ = std::make_shared<json::Writer>(
            this->current_path(),
            std::ios_base::app);
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

            types::TaggedValueList fields =
                !this->keys().empty() ? item->get_fields(this->keys())
                                      : item->as_tvlist();

            this->writer_->write(fields,  // value
                                 false,   // pretty
                                 true);   // newline

            return true;
        }
        else
        {
            return false;
        }
    }
}  // namespace cc::core::logging
