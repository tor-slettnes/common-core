/// -*- c++ -*-
//==============================================================================
/// @file sqlitesink.h++
/// @brief Log tabular data to SQLite3 database
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-sqlite3-sink.h++"
#include "sqlcommand.h++"
#include "logging/message/message.h++"

namespace logger
{
    //--------------------------------------------------------------------------
    // SQLiteSink

    SQLiteSink::SQLiteSink(const std::string &sink_id)
        : AsyncLogSink(sink_id),
          TabularData(),
          RotatingPath(sink_id, ".db")
    {
    }

    void SQLiteSink::load_settings(const core::types::KeyValueMap &settings)
    {
        Super::load_settings(settings);
        this->load_columns(settings);
        this->load_rotation(settings);
    }

    std::optional<core::logging::ColumnSpec> SQLiteSink::column_spec(const core::types::Value &column_data)
    {
        if (const core::types::ValueListPtr colspec = column_data.get_valuelist())
        {
            return core::logging::ColumnSpec({
                .event_field = colspec->get(0).as_string(),
                .column_name = colspec->get(1).as_string(),
                .column_type = colspec->get_as<core::logging::ColumnType>(2)
                                   .value_or(core::logging::ColumnType::TEXT),
                .format_string = colspec->get(3).as_string(),
            });
        }
        else if (column_data.is_string())
        {
            return core::logging::ColumnSpec({
                .event_field = column_data.as_string(),
            });
        }
        else
        {
            return {};
        }
    }

    void SQLiteSink::open()
    {
        this->open_file(this->last_aligned(core::dt::Clock::now()));
        AsyncLogSink::open();
    }

    void SQLiteSink::close()
    {
        AsyncLogSink::close();
        this->close_file();
    }

    void SQLiteSink::open_file(const core::dt::TimePoint &tp)
    {
        RotatingPath::open_file(tp);
        SQLite3::open(this->current_path());
        this->create_table();
    }

    void SQLiteSink::close_file()
    {
        SQLite3::close();
        RotatingPath::close_file();
    }

    void SQLiteSink::create_table()
    {
        std::stringstream header;
        std::stringstream placeholders;
        std::string delimiter;

        placeholders << "(";
        for (const core::logging::ColumnSpec &spec : this->columns())
        {
            header << delimiter
                   << (!spec.column_name.empty()
                           ? spec.column_name
                           : spec.event_field)
                   << " "
                   << spec.column_type;

            placeholders << delimiter
                         << "?";

            delimiter = ", ";
        }
        placeholders << ")";

        this->execute(core::str::format(
            "CREATE TABLE IF NOT EXISTS %s (%s)",
            this->table_name,
            header.rdbuf()));

        this->placeholders = placeholders.str();
    }

    void SQLiteSink::capture_event(const core::status::Event::ptr &event)
    {
        this->check_rotation(event->timepoint());
        const core::types::KeyValueMap &kvmap = event->as_kvmap();
        core::types::ValueList row;

        for (const core::logging::ColumnSpec &spec : this->columns())
        {
            row.push_back(kvmap.get(spec.event_field));
        }
    }

}  // namespace logger
