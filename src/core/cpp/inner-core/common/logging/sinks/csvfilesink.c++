/// -*- c++ -*-
//==============================================================================
/// @file csvfilesink.h++
/// @brief Log tabular data to file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "csvfilesink.h++"
#include "../message/message.h++"
#include "string/misc.h++"

namespace core::logging
{
    //--------------------------------------------------------------------------
    // CSVBaseSink

    CSVBaseSink::CSVBaseSink(const std::string &sink_id)
        : AsyncLogSink(sink_id),
          TabularData(),
          RotatingPath(sink_id, ".csv"),
          separator_(DEFAULT_COL_SEP)
    {
    }

    void CSVBaseSink::load_settings(const types::KeyValueMap &settings)
    {
        Super::load_settings(settings);
        this->load_columns(settings);
        this->load_rotation(settings);

        if (const core::types::Value &value = settings.get(SETTING_COL_SEP))
        {
            this->set_separator(value.as_string());
        }
    }

    void CSVBaseSink::open()
    {
        this->open_file(dt::Clock::now());
        AsyncLogSink::open();
    }

    void CSVBaseSink::close()
    {
        AsyncLogSink::close();
        this->close_file();
    }

    void CSVBaseSink::open_file(const dt::TimePoint &tp)
    {
        RotatingPath::open_file(tp);
        this->stream_ = std::make_shared<std::ofstream>(this->current_path(), std::ios::ate);
        this->stream_->exceptions(std::ios::failbit);

        if (this->stream_->tellp() == 0)
        {
            this->write_header();
        }
    }

    void CSVBaseSink::close_file()
    {
        if (this->stream_)
        {
            this->stream_->close();
            this->stream_.reset();
        }
        RotatingPath::close_file();
    }

    void CSVBaseSink::capture_event(const status::Event::ptr &event)
    {
        if (this->stream_ && this->stream_->good())
        {
            this->check_rotation(event->timepoint());

            auto &stream = *this->stream_;
            const core::types::KeyValueMap &kvmap = event->as_kvmap();
            std::string separator;

            for (const ColumnSpec &spec : this->columns())
            {
                stream << separator;
                separator = this->separator();

                core::types::Value value = kvmap.get(spec.event_field);
                core::str::escape(
                    stream,
                    this->protect_separator(
                        !spec.format_string.empty()
                            ? str::format(spec.format_string, value)
                            : value.as_string()));
            }

            stream << std::endl;
        }
    }

    void CSVBaseSink::write_header()
    {
        std::vector<std::string> columns = this->column_names();
        core::str::join(*this->stream_,     // out
                        columns.begin(),    // begin
                        columns.end(),      // end
                        this->separator(),  // delimiter
                        true,               // keep_empties
                        false);             // quoted
        *this->stream_ << std::endl;
    }

    const std::string &CSVBaseSink::separator() const
    {
        return this->separator_;
    }

    void CSVBaseSink::set_separator(const std::string &separator)
    {
        this->separator_ = separator;
    }

    std::string CSVBaseSink::protect_separator(std::string &&field) const
    {
        if (field.find(this->separator()) != std::string::npos)
        {
            str::substitute("\"", "\"\"", &field);
            return "\"" + field + "\"";
        }
        else
        {
            return field;
        }
    }

    //--------------------------------------------------------------------------
    // CSVEventSink

    CSVEventSink::CSVEventSink(const std::string &sink_id)
        : Super(sink_id)
    {
    }

    //--------------------------------------------------------------------------
    // CSVMessageSink

    CSVMessageSink::CSVMessageSink(const std::string &sink_id)
        : Super(sink_id)
    {
    }

    bool CSVMessageSink::is_applicable(const types::Loggable &item) const
    {
        return this->is_message(item) && Super::is_applicable(item);
    }

}  // namespace core::logging
