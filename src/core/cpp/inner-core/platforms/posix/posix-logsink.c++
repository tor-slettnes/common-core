/// -*- c++ -*-
//==============================================================================
/// @file posix-logsink.c++
/// @brief OS native logger backend - POSIX variant
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-logsink.h++"
#include <syslog.h>

namespace core::platform
{
    PosixLogSinkProvider::PosixLogSinkProvider(
        const std::string &application_id,
        const std::string &sink_id)
        : LogSinkProvider("PosixLogSinkProvider", sink_id),
          application_id_(application_id)
    {
    }

    void PosixLogSinkProvider::open()
    {
        Super::open();
        ::openlog(this->application_id().c_str(), // ident
                  LOG_NDELAY | LOG_PID,           // option
                  LOG_DAEMON);                    // facility
    }

    void PosixLogSinkProvider::close()
    {
        ::closelog();
        Super::close();
    }

    void PosixLogSinkProvider::capture_event(const status::Event::ptr &event)
    {
        if (std::optional<int> level = levelmap.get_opt(event->level()))
        {
            ::syslog(level.value(),                   // priority
                     "%s",                            // format
                     this->formatted(event).c_str()); // args...
        }
    }

    std::string PosixLogSinkProvider::application_id() const
    {
        return this->application_id_;
    }

    const types::ValueMap<status::Level, int> PosixLogSinkProvider::levelmap = {
        {status::Level::DEBUG,    LOG_DEBUG  },
        {status::Level::INFO,     LOG_INFO   },
        {status::Level::NOTICE,   LOG_NOTICE },
        {status::Level::WARNING,  LOG_WARNING},
        {status::Level::FAILED,   LOG_ERR    },
        {status::Level::CRITICAL, LOG_CRIT   },
        {status::Level::FATAL,    LOG_EMERG  },
    };

} // namespace core::platform
