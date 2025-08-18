/// -*- c++ -*-
//==============================================================================
/// @file tabulardata.c++
/// @brief Generic log sink that receives well-known data fields per contract
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tabulardata.h++"
#include "types/symbolmap.h++"
#include "status/event.h++"
#include "status/level.h++"

namespace core::logging
{
    std::ostream &operator<<(std::ostream &stream, const ColumnSpec &spec)
    {
        if (spec.column_name && (spec.column_name.value() != spec.field_name))
        {
            stream << spec.column_name.value()
                   << "=";
        }
        stream << spec.field_name;
        return stream;
    }

    types::TaggedValueList &operator<<(types::TaggedValueList &tvlist,
                                       const ColumnSpec &spec)
    {
        tvlist.append("field_name", spec.field_name);
        tvlist.append_if_value("column_name", spec.column_name);
        tvlist.append("column_type", core::types::TypeNames.try_to_string(spec.column_type));

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

    void TabularData::load_level_map(const core::types::KeyValueMap &settings)
    {
        for (const auto &[key, value] : settings.get(SETTING_LEVEL_MAP).get_kvmap())
        {
            if (auto level = core::str::try_convert_to<core::status::Level>(key))
            {
                this->level_map_.insert_or_assign(level.value(), value);
            }
        }
    }

    void TabularData::load_columns(const types::KeyValueMap &settings)
    {
        if (auto column_list = settings.get(SETTING_COLUMNS).get_valuelist_ptr())
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
            names.push_back(spec.column_name.value_or(spec.field_name));
        }
        return names;
    }

    std::optional<ColumnSpec> TabularData::column_spec(
        const types::Value &column_data) const
    {
        if (const core::types::ValueListPtr colspec = column_data.get_valuelist_ptr())
        {
            return ColumnSpec({
                .field_name = colspec->get(0).as_string(),
                .column_name = colspec->try_get_as<std::string>(1),
                .column_type = core::types::TypeNames.from_string(
                    colspec->get(2).as_string(),
                    core::types::ValueType::STRING),
            });
        }
        else if (column_data.is_string())
        {
            return ColumnSpec({
                .field_name = column_data.as_string(),
                .column_type = core::types::ValueType::STRING,
            });
        }
        else
        {
            return {};
        }
    }

    types::ValueList TabularData::row_data(types::Loggable::ptr item,
                                           bool use_local_time) const
    {
        types::ValueList row;
        for (const core::logging::ColumnSpec &spec : this->columns())
        {
            row.push_back(this->column_data(spec, item, use_local_time));
        }
        return row;
    }

    types::Value TabularData::column_data(const logging::ColumnSpec &spec,
                                          types::Loggable::ptr item,
                                          bool use_local_time) const
    {
        if (spec.field_name == types::Loggable::FIELD_TIME)
        {
            return this->time_value(item->timepoint(),
                                    spec.column_type,
                                    use_local_time);
        }

        else if (spec.field_name == status::Event::FIELD_LEVEL)
        {
            if (auto event = std::dynamic_pointer_cast<status::Event>(item))
            {
                return this->level_value(event->level(),
                                         spec.column_type);
            }
        }

        return item->get_field_as_value(spec.field_name);
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
