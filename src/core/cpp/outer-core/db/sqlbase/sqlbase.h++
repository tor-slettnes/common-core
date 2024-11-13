/// -*- c++ -*-
//==============================================================================
/// @file sqlbase.h++
/// @brief Basic SQL language interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/filesystem.h++"
#include "types/value.h++"
#include "thread/blockingqueue.h++"


namespace core::db
{
    class SQLBase
    {
    protected:
        using ColumnNames = std::vector<std::string>;

        using QueryCallbackFunction = std::function<bool(core::types::TaggedValueList &&)>;
        using QueryResponseQueue = core::types::BlockingQueue<core::types::TaggedValueList>;

        using RowData = core::types::ValueList;
        using MultiRowData = std::vector<RowData>;

    public:

        virtual void execute(
            const std::string &sql,
            const QueryCallbackFunction &callback = {}) const;

        virtual void execute(
            const std::string &sql,
            const RowData &parameters,
            const QueryCallbackFunction &callback = {}) const;

        virtual void execute_multi(
            const std::string &sql,
            const MultiRowData &parameters,
            const QueryCallbackFunction &callback = {}) const = 0;

        std::shared_ptr<QueryResponseQueue> execute_async_query(
            const std::string &sql,
            const RowData &parameters = {},
            std::size_t queue_size = 4096) const;
    };
}  // namespace core::db
