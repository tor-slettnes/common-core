/// -*- c++ -*-
//==============================================================================
/// @file csvfilesink.c++
/// @brief Log tabular data to file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "csvfilesink.h++"
#include "factory.h++"
#include "string/misc.h++"
#include "status/exceptions.h++"

namespace core::logging
{
    //--------------------------------------------------------------------------
    // CSVFileSink

    CSVFileSink::CSVFileSink(const std::string &sink_id)
        : AsyncLogSink(sink_id),
          TabularData(),
          RotatingPath(sink_id, ".csv"),
          separator_(DEFAULT_COL_SEP)
    {
        this->stream.exceptions(std::ios::failbit);
    }

    void CSVFileSink::load_settings(const types::KeyValueMap &settings)
    {
        Super::load_settings(settings);
        this->load_columns(settings);
        this->load_rotation(settings);

        if (const core::types::Value &value = settings.get(SETTING_COL_SEP))
        {
            this->set_separator(value.as_string());
        }
    }

    void CSVFileSink::open()
    {
        this->open_file(dt::Clock::now());
        AsyncLogSink::open();
    }

    void CSVFileSink::close()
    {
        AsyncLogSink::close();
        this->close_file();
    }

    void CSVFileSink::open_file(const dt::TimePoint &tp)
    {
        RotatingPath::open_file(tp);

        this->stream.open(this->current_path(), std::ios::ate);

        if (this->stream.tellp() == 0)
        {
            this->write_header();
        }
    }

    void CSVFileSink::close_file()
    {
        RotatingPath::close_file();

        if (this->stream.is_open())
        {
            this->stream.close();
        }
    }

    void CSVFileSink::capture_event(const status::Event::ptr &event)
    {
        if (this->stream.good())
        {
            this->check_rotation(event->timepoint());
            std::string separator;

            for (types::Value value : this->row_data(event, this->use_local_time()))
            {
                this->stream << separator;
                separator = this->separator();

                str::escape(
                    this->stream,
                    this->protect_separator(value.as_string()));
            }

            this->stream << std::endl;
        }
    }

    void CSVFileSink::write_header()
    {
        std::vector<std::string> columns = this->column_names();
        str::join(this->stream,       // out
                  columns.begin(),    // begin
                  columns.end(),      // end
                  this->separator(),  // delimiter
                  true,               // keep_empties
                  false);             // quoted
        this->stream << std::endl;
    }

    const std::string &CSVFileSink::separator() const
    {
        return this->separator_;
    }

    void CSVFileSink::set_separator(const std::string &separator)
    {
        this->separator_ = separator;
    }

    std::string CSVFileSink::protect_separator(std::string &&field) const
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
}  // namespace core::logging
