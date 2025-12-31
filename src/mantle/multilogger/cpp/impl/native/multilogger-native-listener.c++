/// -*- c++ -*-
//==============================================================================
/// @file multilogger-native-listener.c++
/// @brief A server-side log sink appending to a queue
/// @author Tor Slettnes
//==============================================================================

#include "multilogger-native-listener.h++"
#include "platform/path.h++"
#include "logging/dispatchers/dispatcher.h++"
#include "logging/message/message.h++"

namespace multilogger::native
{
    QueueListener::QueueListener(
        const SinkID &sink_id,
        core::status::Level threshold,
        const std::optional<Loggable::ContractID> &contract_id,
        const std::set<std::string> &hosts,
        const std::set<std::string> &applications,
        unsigned int maxsize,
        OverflowDisposition overflow_disposition)
        : Sink(sink_id),
          BlockingQueue(maxsize, overflow_disposition),
          hosts(hosts),
          applications(applications)
    {
        this->set_threshold(threshold);
        this->set_contract_id(contract_id);

        core::platform::signal_shutdown.connect(
            this->sink_id(),
            std::bind(&QueueListener::close, this));
    }

    QueueListener::~QueueListener()
    {
        core::platform::signal_shutdown.disconnect(this->sink_id());
    }

    void QueueListener::open()
    {
        Sink::open();
        core::logging::dispatcher.add_sink(this->shared_from_this());
    }

    void QueueListener::close()
    {
        core::logging::dispatcher.remove_sink(this->shared_from_this());
        BlockingQueue::close();
        Sink::close();
    }

    bool QueueListener::handle_item(const core::types::Loggable::ptr &item)
    {
        if (auto message = std::dynamic_pointer_cast<core::logging::Message>(item))
        {
            if (this->applicable_host(message->host()) &&
                this->applicable_application(message->origin()))
            {
                return this->put(item);
            }
            else
            {
                return false;
            }
        }
        else
        {
            return this->put(item);
        }
    }

    bool QueueListener::applicable_host(const std::string &host) const
    {
        return this->hosts.empty() || this->hosts.count(host);
    }

    bool QueueListener::applicable_application(const std::string &application) const
    {
        return (application != core::platform::path->exec_name()) &&
               (this->applications.empty() || this->applications.count(application));
    }

}  // namespace multilogger::native
