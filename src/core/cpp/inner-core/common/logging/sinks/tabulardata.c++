/// -*- c++ -*-
//==============================================================================
/// @file tabulardata.c++
/// @brief Generic log sink that receives well-known data fields per contract
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tabulardata.h++"
#include "types/symbolmap.h++"
#include "../message/message.h++"

namespace core::logging
{
    types::SymbolMap<core::types::ValueType> column_type_names = {
        {core::types::ValueType::NONE, "NULL"},
        {core::types::ValueType::BOOL, "BOOLEAN"},
        {core::types::ValueType::SINT, "INTEGER"},
        {core::types::ValueType::REAL, "REAL"},
        {core::types::ValueType::STRING, "TEXT"},
        {core::types::ValueType::BYTEVECTOR, "BLOB"},
        {core::types::ValueType::TIMEPOINT, "DATETIME"},
    };

    std::ostream &operator<<(std::ostream &stream, const ColumnSpec &spec)
    {
        if (spec.column_name && (spec.column_name.value() != spec.event_field))
        {
            stream << spec.column_name.value()
                   << "=";
        }
        stream << spec.event_field;
        return stream;
    }

    types::TaggedValueList &operator<<(types::TaggedValueList &tvlist,
                                       const ColumnSpec &spec)
    {
        tvlist.append("event_field", spec.event_field);
        tvlist.append_if_value("column_name", spec.column_name);
        tvlist.append("column_type", column_type_names.to_string(spec.column_type));

        return tvlist;
    }

    types::ValueList &operator<<(types::ValueList &valuelist,
                                 const ColumnSpecs &specs)
    {
        for (const ColumnSpec &spec : specs)
        {
            valuelist.push_back(
                types::TaggedValueList::create_shared_from(spec));
        }
        return valuelist;
    }

    //--------------------------------------------------------------------------
    // TabularData

    TabularData::TabularData(const ColumnSpecs &columns)
        : columns_(columns)
    {
    }

    ColumnSpecs TabularData::default_columns()
    {
        std::vector<std::string> field_names = Message::message_fields();

        ColumnSpecs specs;
        specs.reserve(field_names.size());
        for (const std::string &field_name : field_names)
        {
            specs.push_back({
                .event_field = field_name,
                .column_name = str::toupper(field_name),
                .column_type = types::ValueType::STRING,
            });
        }
        return specs;
    }

    void TabularData::load_level_map(const core::types::KeyValueMap &settings)
    {
        if (core::types::KeyValueMapPtr map = settings.get(SETTING_LEVEL_MAP).get_kvmap())
        {
            for (const auto &[key, value] : *map)
            {
                if (auto level = core::str::try_convert_to<core::status::Level>(key))
                {
                    this->level_map_.insert_or_assign(level.value(), value);
                }
            }
        }
    }

    void TabularData::load_columns(const types::KeyValueMap &settings)
    {
        if (auto column_list = settings.get(SETTING_COLUMNS).get_valuelist())
        {
            ColumnSpecs specs;
            specs.reserve(column_list->size());

            for (const core::types::Value &column_data : *column_list)
            {
                if (auto spec = this->column_spec(column_data))
                {
                    specs.push_back(spec.value());
                }
            }
            this->set_columns(specs);
        }
    }

    const TabularData::LevelMap &TabularData::level_map() const
    {
        return this->level_map_;
    }

    void TabularData::set_level_map(const LevelMap &level_map)
    {
        this->level_map_ = level_map;
    }

    const ColumnSpecs &TabularData::columns() const
    {
        return this->columns_;
    }

    void TabularData::set_columns(const ColumnSpecs &columns)
    {
        this->columns_ = columns;
    }

    std::vector<std::string> TabularData::column_names() const
    {
        std::vector<std::string> names;
        names.reserve(this->columns().size());
        for (const ColumnSpec &spec : this->columns())
        {
            names.push_back(spec.column_name.value_or(spec.event_field));
        }
        return names;
    }

    std::optional<ColumnSpec> TabularData::column_spec(
        const types::Value &column_data) const
    {
        if (const core::types::ValueListPtr colspec = column_data.get_valuelist())
        {
            return ColumnSpec({
                .event_field = colspec->get(0).as_string(),
                .column_name = colspec->try_get_as<std::string>(1),
                .column_type = column_type_names.from_string(
                    colspec->get(2).to_string(),
                    core::types::ValueType::STRING),
            });
        }
        else if (column_data.is_string())
        {
            return ColumnSpec({
                .event_field = column_data.as_string(),
                .column_type = core::types::ValueType::STRING,
            });
        }
        else
        {
            return {};
        }
    }

    types::ValueList TabularData::row_data(status::Event::ptr event,
                                           bool use_local_time) const
    {
        types::ValueList row;
        for (const core::logging::ColumnSpec &spec : this->columns())
        {
            row.push_back(this->column_data(spec, event, use_local_time));
        }
        return row;
    }

    types::Value TabularData::column_data(const logging::ColumnSpec &spec,
                                          status::Event::ptr event,
                                          bool use_local_time) const
    {
        if (spec.event_field == status::EVENT_FIELD_TIME)
        {
            return this->time_value(event->timepoint(),
                                    spec.column_type,
                                    use_local_time);
        }
        else if (spec.event_field == status::EVENT_FIELD_LEVEL)
        {
            return this->level_value(event->level(),
                                     spec.column_type);
        }
        else
        {
            return event->get_field_as_value(spec.event_field);
        }
    }

    types::Value TabularData::time_value(const dt::TimePoint &tp,
                                         types::ValueType value_type,
                                         bool use_local_time) const
    {
        switch (value_type)
        {
        case types::ValueType::SINT:
        case types::ValueType::UINT:
            return dt::to_time_t(tp);

        case types::ValueType::REAL:
            return dt::to_double(tp);

        case types::ValueType::STRING:
            return str::format(use_local_time ? "%T" : "%Z", tp);

        default:
            return tp;
        }
    }

    types::Value TabularData::level_value(status::Level level,
                                          types::ValueType value_type) const
    {
        if (const types::Value &mapped_level = this->level_map().get(level))
        {
            return mapped_level;
        }

        else if (types::is_integral(value_type))
        {
            return static_cast<unsigned int>(level);
        }

        else
        {
            return str::convert_from(level);
        }
    }

}  // namespace core::logging
