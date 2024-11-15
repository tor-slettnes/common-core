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
    //--------------------------------------------------------------------------
    // Column specifications

    types::SymbolMap<ColumnType> column_type_names = {
        {ColumnType::NONE, "NULL"},
        {ColumnType::BOOL, "BOOLEAN"},
        {ColumnType::INT, "INTEGER"},
        {ColumnType::REAL, "REAL"},
        {ColumnType::TEXT, "TEXT"},
        {ColumnType::BINARY, "BLOB"},
        {ColumnType::DATETIME, "DATETIME"},
    };

    std::ostream &operator<<(std::ostream &stream, const ColumnType &type)
    {
        return column_type_names.to_stream(stream, type);
    }

    std::istream &operator>>(std::istream &stream, ColumnType &type)
    {
        return column_type_names.from_stream(stream,  // stream
                                             &type,   // key
                                             {},      // fallback
                                             true,    // flag_unknown
                                             true);   // allow_partial
    }

    std::ostream &operator<<(std::ostream &stream, const ColumnSpec &spec)
    {
        if (!spec.column_name.empty() && (spec.column_name != spec.event_field))
        {
            stream << spec.column_name
                   << "=";
        }
        stream << spec.event_field;
        return stream;
    }

    types::TaggedValueList &operator<<(types::TaggedValueList &tvlist,
                                       const ColumnSpec &spec)
    {
        tvlist.append("event_field",
                      spec.event_field);

        tvlist.append_if(!spec.column_name.empty(),
                         "column_name",
                         spec.column_name);

        tvlist.append("column_type",
                      str::convert_from(spec.column_type));

        tvlist.append_if(!spec.format_string.empty(),
                         "format_string",
                         spec.format_string);
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

    std::optional<ColumnSpec> TabularData::column_spec(const types::Value &column_data)
    {
        if (const core::types::ValueListPtr colspec = column_data.get_valuelist())
        {
            return ColumnSpec({
                .event_field = colspec->get(0).as_string(),
                .column_name = colspec->get(1).as_string(),
                .format_string = colspec->get(2).as_string(),
            });
        }
        else if (column_data.is_string())
        {
            return ColumnSpec({
                .event_field = column_data.as_string(),
            });
        }
        else
        {
            return {};
        }
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
            names.push_back(!spec.column_name.empty()
                                ? spec.column_name
                                : spec.event_field);
        }
        return names;
    }

    ColumnSpecs TabularData::default_columns()
    {
        std::vector<std::string> field_names = Message::field_names();

        ColumnSpecs specs;
        specs.reserve(field_names.size());
        for (const std::string &field_name : field_names)
        {
            specs.push_back({
                .event_field = field_name,
                .column_name = str::toupper(field_name),
                .column_type = ColumnType::TEXT,
            });
        }
        return specs;
    }
}  // namespace core::logging
