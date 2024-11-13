/// -*- c++ -*-
//==============================================================================
/// @file sqlbase.h++
/// @brief Basic SQL language interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sqlbase.h++"

#include <functional>
#include <future>

namespace core::db
{
    void SQLBase::execute(
        const std::string &sql,
        const QueryCallbackFunction &callback) const
    {
        this->execute(sql, {}, callback);
    }

    void SQLBase::execute(
        const std::string &sql,
        const RowData &parameters,
        const QueryCallbackFunction &callback) const
    {
        this->execute_multi(sql, {parameters}, callback);
    }

    std::shared_ptr<SQLBase::QueryResponseQueue> SQLBase::execute_async_query(
        const std::string &sql,
        const RowData &parameters,
        std::size_t queue_size) const
    {
        using namespace std::placeholders;

        auto queue = std::make_shared<QueryResponseQueue>(
            queue_size,
            QueryResponseQueue::OverflowDisposition::BLOCK);

        std::thread executor_thread([=] {
            this->execute(sql, parameters, [=](core::types::TaggedValueList &&row) -> bool {
                return queue->put(row);
            });
            queue->close();
        });

        executor_thread.detach();
        return queue;
    }

}  // namespace core::db
