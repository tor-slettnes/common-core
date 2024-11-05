// -*- c++ -*-
//==============================================================================
/// @file logger-api.c++
/// @brief Logging service - abstract base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-api.h++"

namespace logger
{
    API::API(const std::string &identity)
        : identity_(identity),
          keep_listening_(false)
    {
    }

    API::~API()
    {
        this->stop_listening();
    }

    const std::string &API::identity() const
    {
        return this->identity_;
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

    void API::stop_listening()
    {
        this->keep_listening_ = false;

        if (auto listener = this->listener_.lock())
        {
            listener->close();
        }

        if (this->listener_thread_.joinable())
        {
            this->listener_thread_.join();
        }
    }

    void API::keep_listening(const ListenerSpec &spec)
    {
        while (this->keep_listening_)
        {
            std::shared_ptr<EventSource> listener = this->listen(spec);
            this->listener_ = listener;

            while (std::optional<core::status::Event::ptr> event = listener->get())
            {
                signal_log_event.emit(event.value());
            }
        }
    }

    core::signal::DataSignal<core::status::Event::ptr> signal_log_event("signal_log_event");

}  // namespace logger
