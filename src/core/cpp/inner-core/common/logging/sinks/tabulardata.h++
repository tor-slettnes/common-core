/// -*- c++ -*-
//==============================================================================
/// @file tabulardata.h++
/// @brief Generic log sink that receives well-known data fields per contract
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "status/level.h++"
#include "types/value.h++"
#include "types/valuemap.h++"
#include "types/symbolmap.h++"
#include "types/loggable.h++"

#include <string>
#include <memory>
#include <vector>
#include <ostream>

namespace core::logging
{
    const std::string SETTING_COLUMNS = "columns";
    const std::string SETTING_LEVEL_MAP = "level map";

    //--------------------------------------------------------------------------
    // Column specifications

    struct ColumnSpec
    {
        std::string field_name;
        std::optional<std::string> column_name;
        types::ValueType column_type = types::ValueType::NONE;
    };

    extern types::SymbolMap<core::types::ValueType> column_type_names;

    std::ostream &operator<<(std::ostream &stream, const ColumnSpec &spec);
    types::TaggedValueList &operator<<(types::TaggedValueList &tvlist,
                                       const ColumnSpec &spec);

    using ColumnNames = std::vector<std::string>;
    using ColumnSpecs = std::vector<ColumnSpec>;

    types::ValueList &operator<<(types::ValueList &valuelist,
                                 const ColumnSpecs &specs);

    //--------------------------------------------------------------------------
    /// @class TabularData
    /// @brief Abstract base for sinks that log a fixed set of fields

    class TabularData
    {
        using This = TabularData;
        using LevelMap = types::ValueMap<status::Level, types::Value>;

    protected:
        TabularData(const ColumnSpecs &columns = {});

    protected:
        void load_level_map(const types::KeyValueMap &settings);
        void load_columns(const types::KeyValueMap &settings);

    public:
        const ColumnSpecs &columns() const;
        void set_columns(const ColumnSpecs &columns);

        const LevelMap &level_map() const;
        void set_level_map(const LevelMap &level_map);

        std::vector<std::string> column_names() const;

    private:
        std::optional<ColumnSpec> column_spec(const types::Value &column_data) const;

    protected:
        types::ValueList row_data(types::Loggable::ptr loggable,
                                  bool use_local_time) const;

    private:
        types::Value column_data(
            const logging::ColumnSpec &spec,
            types::Loggable::ptr loggable,
            bool use_local_time) const;

        types::Value time_value(
            const dt::TimePoint &tp,
            types::ValueType value_type,
            bool use_local_time) const;

        types::Value level_value(
            status::Level level,
            types::ValueType value_type) const;

    private:
        LevelMap level_map_;
        ColumnSpecs columns_;
    };
}  // namespace core::logging
