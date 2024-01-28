/// -*- c++ -*-
//==============================================================================
/// @file builder.c++
/// @brief Construct a log message using `std::ostream` interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "builder.h++"
#include "buildinfo.h"

namespace cc::logging
{
    MessageBuilder::MessageBuilder(Dispatcher *dispatcher,
                                   Scope::Ref scope,
                                   status::Level level,
                                   status::Flow flow,
                                   const dt::TimePoint &tp,
                                   const fs::path &path,
                                   const uint &lineno,
                                   const std::string &function,
                                   pid_t thread_id)
        : Message({},                                                // text
                  scope,                                             // scope
                  level,                                             // level
                  flow,                                              // flow
                  tp,                                                // tp
                  fs::relative(path, SOURCE_DIR, this->path_error),  // path
                  lineno,                                            // lineno
                  function,                                          // function
                  thread_id,                                         // thread_id
                  {},                                                // origin
                  0,                                                 // code
                  {},                                                // symbol
                  {}),                                               // attributes
          dispatcher_(dispatcher),
          is_applicable_(Message::is_applicable() && dispatcher->is_applicable(*this))
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
}  // namespace cc::logging
