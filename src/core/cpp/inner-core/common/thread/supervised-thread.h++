/// -*- c++ -*-
//==============================================================================
/// @file supervised-thread.h++
/// @brief Spawn a new thread with exception handler
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "logging/logging.h++"
#include "platform/process.h++"
#include "status/exceptions.h++"

#include <thread>
#include <functional>
#include <iostream>

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
                // std::cerr << "Thread "
                //           << platform::process->thread_id()
                //           << " terminated: "
                //           << std::current_exception()
                //           << std::endl;
                log_error("Thread terminated: ",
                          std::current_exception());
                // std::exit(-1);
            }
        });
    }
}  // namespace core::thread
