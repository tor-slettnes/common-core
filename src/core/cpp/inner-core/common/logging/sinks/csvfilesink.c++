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

    CSVBaseSink::CSVBaseSink(const std::string &sink_id,
                             status::Level threshold,
                             const std::optional<std::string> &contract_id,
                             const ColumnDefaults &columns,
                             const std::string &path_template,
                             const dt::DateTimeInterval &rotation_interval,
                             bool local_time,
                             const std::string &separator)
        : TabularDataSink(sink_id, threshold, contract_id, columns),
          RotatingPath(sink_id, path_template, ".csv", rotation_interval, local_time),
          separator_(separator)
    {
    }

    const std::string &CSVBaseSink::separator() const
    {
        return this->separator_;
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

            for (const auto &[column_name, default_value] : this->column_defaults())
            {
                stream << separator;
                separator = this->separator();

                const core::types::Value &value = kvmap.get(
                    column_name.value_or(""),
                    default_value);

                core::str::escape(stream, this->protect_separator(value.as_string()));
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
    // CSVMessageSink

    CSVMessageSink::CSVMessageSink(const std::string &sink_id,
                                   status::Level threshold,
                                   const std::vector<std::string> &column_names,
                                   const std::string &path_template,
                                   const dt::DateTimeInterval &rotation_interval,
                                   bool local_time,
                                   const std::string &separator)
        : Super(sink_id,                              // sink_id
                threshold,                            // threshold
                {},                                   // contract_id
                This::message_columns(column_names),  // columns
                path_template,                        // path_template
                rotation_interval,                    // rotation_interval
                local_time,                           // local_time
                separator)                            // separator
    {
    }

    bool CSVMessageSink::is_applicable(const types::Loggable &item) const
    {
        bool is_message = dynamic_cast<const logging::Message *>(&item) != nullptr;
        return is_message && AsyncLogSink::is_applicable(item);
    }

    types::TaggedValueList CSVMessageSink::message_columns(
        const std::vector<std::string> &column_names)
    {
        std::vector<std::string> field_names = !column_names.empty()
                                                   ? column_names
                                                   : Message::field_names();
        types::TaggedValueList tvlist;
        for (const std::string &field_name : field_names)
        {
            tvlist.push_back({field_name, ""});
        }

        return tvlist;
    }

}  // namespace core::logging
