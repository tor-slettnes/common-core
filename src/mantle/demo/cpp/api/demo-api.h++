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

namespace demo
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
            const core::dt::TimePoint &birth = core::dt::Clock::now());

    public:
        virtual void initialize() {}
        virtual void deinitialize() {}

        /// @brief Issue a greeting to anyone who may be listening
        /// @param[in] message
        ///     What we're telling our listeners
        /// @param[in] data
        ///     Arbitrary key/value items to include in greeting
        void say_hello(const std::string &message,
                       const core::types::KeyValueMap &data = {});

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
        ///     to `demo::signal_time`; see `demo-signals.hpp` for details.
        virtual void start_ticking() = 0;

        /// @brief Stop any existing time notification task
        virtual void stop_ticking() = 0;

        /// @brief Inform implementation that we want to receive updates
        /// @note This is not used by every implementation, thus not pure virtual
        virtual void start_watching() {}

        /// @brief Inform implementation that we no longer need updates
        /// @note This is not used by every implementation, thus not pure virtual
        virtual void stop_watching() {}

    public:
        // Accessor methods
        std::string identity() const;
        std::string implementation() const;
        core::dt::TimePoint birth() const;

    protected:
        std::string identity_;
        std::string implementation_;
        core::dt::TimePoint birth_;
    };
}  // namespace demo
