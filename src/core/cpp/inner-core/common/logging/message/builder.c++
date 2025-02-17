/// -*- c++ -*-
//==============================================================================
/// @file builder.c++
/// @brief Construct a log message using `std::ostream` interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "builder.h++"
#include "buildinfo.h"

namespace core::logging
{
    MessageBuilder::MessageBuilder(Dispatcher *dispatcher,
                                   status::Level level,
                                   Scope::ptr scope,
                                   const dt::TimePoint &tp,
                                   const fs::path &path,
                                   const uint &lineno,
                                   const std::string &function,
                                   pid_t thread_id)
        : Message({},                                                      // text
                  level,                                                   // level
                  scope,                                                   // scope
                  tp,                                                      // tp
                  fs::relative(path, SOURCE_DIR, this->path_error),        // path
                  lineno,                                                  // lineno
                  function,                                                // function
                  thread_id,                                               // thread_id
                  platform::host ? platform::host->get_host_name() : ""s,  // host
                  platform::path ? platform::path->exec_name() : ""s),     // origin
          dispatcher_(dispatcher),
          is_applicable_(Message::is_applicable() &&
                         dispatcher->is_applicable(*this))
    {
    }

    std::string MessageBuilder::text() const noexcept
    {
        return this->str();
    }

    bool MessageBuilder::is_applicable() const noexcept
    {
        return this->is_applicable_;
    }

    void MessageBuilder::dispatch()
    {
        if (this->is_applicable())
        {
            this->dispatcher_->submit(this->shared_from_this());
        }
    }
}  // namespace core::logging
