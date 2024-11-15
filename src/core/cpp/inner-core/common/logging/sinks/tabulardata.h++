/// -*- c++ -*-
//==============================================================================
/// @file tabulardata.h++
/// @brief Generic log sink that receives well-known data fields per contract
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"

#include <string>
#include <memory>
#include <vector>
#include <ostream>

namespace core::logging
{
    const std::string SETTING_COLUMNS = "columns";

    //--------------------------------------------------------------------------
    // Column specifications

    enum class ColumnType
    {
        NONE,
        BOOL,
        INT,
        REAL,
        TEXT,
        BINARY,
        DATETIME
    };

    std::ostream &operator<<(std::ostream &stream, const ColumnType &type);
    std::istream &operator>>(std::istream &stream, ColumnType &type);

   struct ColumnSpec
    {
        std::string event_field;
        std::string column_name;
        ColumnType column_type = ColumnType::NONE;
        std::string format_string;
    };

    std::ostream &operator<<(std::ostream &stream, const ColumnSpec &spec);

    types::TaggedValueList &operator<<(types::TaggedValueList &tvlist,
                                       const ColumnSpec &spec);


    using ColumnNames = std::vector<std::string>;
    using ColumnSpecs = std::vector<ColumnSpec>;

    types::ValueList &operator<<(types::ValueList &valuelist,
                                 const ColumnSpecs &specs);

    //--------------------------------------------------------------------------
    /// \class TabularData
    /// \brief Abstract base for sinks that log a fixed set of fields

    class TabularData
    {
        using This = TabularData;

    protected:
        TabularData(const ColumnSpecs &columns = This::default_columns());

    protected:
        virtual void load_columns(const types::KeyValueMap &settings);

    private:
        virtual std::optional<ColumnSpec> column_spec(const types::Value &column_data);

    public:
        const ColumnSpecs &columns() const;
        void set_columns(const ColumnSpecs &columns);

        std::vector<std::string> column_names() const;

    private:
        static ColumnSpecs default_columns();

    private:
        ColumnSpecs columns_;
    };
}  // namespace core::logging
