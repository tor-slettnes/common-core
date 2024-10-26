/// -*- c++ -*-
//==============================================================================
/// @file csvfilesink.h++
/// @brief Log tabular data to file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "csvfilesink.h++"
#include "string/misc.h++"

namespace core::logging
{
    CSVFileSink::CSVFileSink(const std::string contract_id,
                             const ColumnDefaults &column_template,
                             const std::string &path_template,
                             const dt::Duration &rotation_interval)
        : Super(contract_id, column_template),
          RotatingPath(path_template, ".csv", rotation_interval)
    {
    }

    void CSVFileSink::open(const dt::TimePoint &tp)
    {
        this->update_current_path(tp);
        this->stream_ = std::make_shared<std::ofstream>(this->current_path(), std::ios::ate);
        this->stream_->exceptions(std::ios::failbit);

        if (this->stream_->tellp() > 0)
        {
            this->write_header();
        }
    }

    void CSVFileSink::close()
    {
        if (this->stream_)
        {
            this->stream_->close();
            this->stream_.reset();
        }
    }

    void CSVFileSink::capture_event(const status::Event::ptr &event)
    {
        if (this->stream_ && this->stream_->good())
        {
            this->check_rotation(event->timepoint());

            auto &stream = *this->stream_;
            std::string separator;

            for (const auto &[column_name, default_value] : this->column_defaults())
            {
                stream << separator;

                core::types::Value value = event->attribute(
                    column_name.value_or(""),
                    default_value);

                value.to_literal_stream(stream);
            }

            stream << std::endl;
        }
    }

    void CSVFileSink::write_header()
    {
        std::vector<std::string> columns = this->column_names();
        core::str::join(*this->stream_,   // out
                        columns.begin(),  // begin
                        columns.end(),    // end
                        ",",              // delimiter
                        true,             // keep_empties
                        false);           // quoted
    }

}  // namespace core::logging
