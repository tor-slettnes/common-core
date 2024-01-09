// -*- c++ -*-
//==============================================================================
/// @file demo-api.h++
/// @brief Example app - Abstract C++ interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
// Application specific modules
#include "demo-types.h++"

// Shared modules
#include "chrono/date-time.h++"

namespace cc::demo
{
    //==========================================================================
    /// @class API
    /// @brief Abstract API, common to all implementations.

    class API
    {
    protected:
        /// @brief Constructor. This will be invoked only from derived classes.
        API(const std::string &identity,
            const std::string &implementation,
            const cc::dt::TimePoint &birth = cc::dt::Clock::now());

    public:
        virtual void initialize() {}
        virtual void deinitialize() {}

        /// @brief Issue a greeting to anyone who may be listening
        /// @param[in] greeting
        ///     What we're telling our listeners
        /// @note​
        ///     To receive greetings from others, connect a callback
        ///     function ("slot") ​to `demo::signal_greeting`.​
        virtual void say_hello(const Greeting &greeting) = 0;

        /// @brief Get current time data.
        /// @return
        ///     Current time data provided by the specific implementation.
        virtual TimeData get_current_time() = 0;

        /// @brief Start periodic time notification task
        /// @note
        ///     To receive time updates, connect a callback function ("slot")
        ///     to `cc::demo::signal_time`; see `demo-signals.hpp` for details.
        virtual void start_ticking() = 0;

        /// @brief Stop any existing time notification task
        virtual void stop_ticking() = 0;

        /// @brief Inform implementation that we want to receive updates
        virtual void start_watching() {}

        /// @brief Inform implementation that we no longer need updates
        virtual void stop_watching() {}

    public:
        // Public interface
        void say_hello(const std::string &greeting,
                       const cc::types::KeyValueMap &data = {});

        // Accessor methods
        std::string identity() const;
        std::string implementation() const;
        cc::dt::TimePoint birth() const;

    protected:
        std::string identity_;
        std::string implementation_;
        cc::dt::TimePoint birth_;
    };
}  // namespace cc::demo
