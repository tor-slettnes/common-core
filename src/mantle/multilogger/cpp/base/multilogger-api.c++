// -*- c++ -*-
//==============================================================================
/// @file multilogger-api.c++
/// @brief Logging service - abstract base
/// @author Tor Slettnes
//==============================================================================

#include "multilogger-api.h++"

namespace multilogger
{
    API::API()
        : keep_listening_(false)
    {
    }

    API::~API()
    {
        this->stop_listening();
    }

    void API::start_listening(const ListenerSpec &spec)
    {
        this->keep_listening_ = true;
        if (!this->listener_thread_.joinable())
        {
            this->listener_thread_ = std::thread(
                &API::keep_listening,
                this,
                spec);
        }
    }

    void API::stop_listening(bool wait)
    {
        this->keep_listening_ = false;

        if (auto listener = this->listener_.lock())
        {
            listener->close();

            if (wait && this->listener_thread_.joinable())
            {
                this->listener_thread_.join();
            }
        }
    }

    void API::keep_listening(const ListenerSpec &spec)
    {
        while (this->keep_listening_)
        {
            std::shared_ptr<LogSource> listener = this->listen(spec);
            this->listener_ = listener;

            while (std::optional<core::types::Loggable::ptr> item = listener->get())
            {
                signal_log_item.emit(item.value());
            }
        }
    }

    core::signal::DataSignal<core::types::Loggable::ptr> signal_log_item("signal_log_item");

}  // namespace multilogger
