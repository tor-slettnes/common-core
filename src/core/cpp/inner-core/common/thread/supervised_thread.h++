/// -*- c++ -*-
//==============================================================================
/// @file supervised_thread.h++
/// @brief Spawn a new thread with exception handler
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logging/logging.h++"
#include "platform/process.h++"

#include <thread>

namespace core::thread
{
    template <class... Args>
    std::thread supervised_thread(Args &&...args)
    {
        return std::thread([=]() {
            try
            {
                return std::invoke(args...);
            }
            catch (...)
            {
                logf_critical("Thread %d terminated: %s",
                              platform::process->thread_id(),
                              std::current_exception());
                std::exit(-1);
            }
        });
    }
}  // namespace core::thread
