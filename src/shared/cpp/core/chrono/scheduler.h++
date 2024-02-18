/// -*- c++ -*-
//==============================================================================
/// @file scheduler.h++
/// @brief schedule tasks to be invoked at specified intervals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "date-time.h++"
#include "logging/logging.h++"

#include <variant>
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <thread>

namespace shared
{
    /// @class Scheduler
    /// @brief Schedule callbacks to be invoked at specified time intervals.
    ///
    /// The Scheduler class manages tasks to be executed at specified time
    /// intervals. Each task must be identified by a unique id and associated with a
    /// callback function, which may be invoked a specified number of times or until
    /// the task is explicitly removed (unscheduled). A retry count specifies how
    /// many times in a row the callback may throw an exception before the task is
    /// implicitly removed, and defaults to zero.
    ///
    /// By default a task's invocation times are aligned to the time point when the
    /// task was added.  Alternatively, invocation times may be aligned to a UTC or
    /// local time reference. The distinction between UTC and local time is only
    /// meaningful if the specified interval does not evenly divide an hour (or in
    /// India, 30 minutes). For instance, a task scheduled every 6 hour aligned to
    /// local time will run at midnight, 6AM, noon, and 6PM in the local timezone.
    /// Additionally, such a task would be subject to local time adjustments,
    /// notably Daylight Savings Time/Summer Time: At the start of DST, the midnight
    /// and 6AM invocations will take place only 5 hours apart, while at the end of
    /// DST there is 7 hours between each invocation.
    ///
    /// (For smaller intervals, UTC alignment may be preferable as it is slightly
    /// simpler).
    ///
    /// The scheduler lazily spawns a single thread in which to invoke callbacks as
    /// required. Thus, while each scheduler instance can manage an unlimited number
    /// of tasks with different time references and intervals, care must be
    /// exercised to prevent tasks from starving each other within this thread. To
    /// schedule tasks that may linger around for a while at each invocation, or
    /// conversely, that are sensitive to jitter from other tasks, consider using a
    /// separate scheduler instance.
    ///
    /// The `catchup` option specifies how to handle missed invocations, i.e., if an
    /// invocation does not take place until after the next scheduled time point.
    /// Normally such an invocation is skipped, however if `catchup` is `true` then
    /// the task's scheduled times remain unmodified. This option should be used
    /// sparingly, since it is also a recipe for starvation.
    ///
    /// Each time the scheduler wakes up to execute a task, it performs a sanity
    /// check on the system time. If the time has skewed more than an acceptable
    /// tolerance (by default, 5 seconds) from where it is expected, presumably
    /// because the system time changed, the invocation times of these tasks are
    /// adjusted accordingly. This also prevents a busy loop to repeatedly invoke
    /// tasks with the `catchup` option in cases where the clock is adjusted
    /// forwardly significantly (e.g. following initial NTP synchronization, or
    /// after having suspended a laptop for some amount of time).

    class Scheduler
    {
        // Forward
        class Task;

        using Invocation = std::variant<
            std::function<void()>,
            std::function<void(const dt::TimePoint &)>,
            std::function<void(const dt::TimePoint &, const Task &)>>;

    public:  // Types
        enum Alignment
        {
            ALIGN_START,  // First invocation takes place immediately
            ALIGN_NEXT,   // First invocation takes place after `interval`
            ALIGN_UTC,    // First invocation aligned to epoch / UTC.
            ALIGN_LOCAL   // Aligned to local timezone offset. Realigns at DST start/end.
        };

        /// Map of scheduled tasks by next scheduled invocation
        using TaskMap = std::multimap<dt::TimePoint, Task>;
        using Handle = std::string;

    public:  // Methods
        /// @fn Scheduler
        /// @brief Constructor
        /// @param[in] max_nap
        ///     Maximum allowed time nap time between checks for task updates.
        /// @param[in] max_jitter
        ///     Maximum allowed time interval between expected and actual system
        ///     clock observed each time the scheduler wakes up to invoke a task.
        ///     If exceeded, task invocation times will adjusted according to the
        ///     difference.

        Scheduler(dt::Duration max_nap = std::chrono::seconds(1),
                  dt::Duration max_jitter = std::chrono::seconds(5));

        /// @fn ~Scheduler
        /// @brief Destructor. Cancel any pending tasks.
        ~Scheduler();

        /// @fn add
        /// @brief Schedule a task/method to be invoked at specified time interval
        /// @param[in] invocation
        ///     Method to invoke
        /// @param[in] interval
        ///     Interval between invocations
        /// @param[in] align
        ///     TimePoint alignment, i.e., when the first invocation takes place. Use
        ///       - \p ALIGN_START to invoke the task as soon as it added to the scheduler,
        ///       - \p ALIGN_NEXT to delay one cycle, until the specified interval has elapsed,
        ///       - \p ALIGN_UTC to align invocations to `interval` multiples from midnight, UTC
        ///         (technically, from midnight January 1, 1970, UTC, a.k.a. epoch).
        ///       - \p ALIGN_LOCAL to align invocations to midnight, local time (technically,
        ///         epoch minus the current local timezone offset).
        /// @param[in] loglevel
        ///     Verbosity at which task invocations are logged
        /// @param[in] count
        ///     Number of invocations; 0 is infinite.
        /// @param[in] retries
        ///     Maximum number of invocation failures to allow before unscheduling task.
        /// @param[in] catchup
        ///     Whether to immediately reschedule tasks that could not be executed in their
        ///     specified timeslot (because a prior invocation task took too long, or or due to
        ///     system load). Using this could lead to starvation.
        /// @return
        ///     Unique handle which can subsequently be used to remove task.
        Handle add(const Invocation &invocation,
                   const dt::Duration &interval,
                   const Alignment align = ALIGN_NEXT,
                   status::Level loglevel = status::Level::DEBUG,
                   uint count = 0,
                   uint retries = 0,
                   bool catchup = false);

        /// @fn add
        /// @brief Schedule a task/method to be invoked at specified time interval
        /// @param[in] id
        ///     Unique identifier for this task.
        /// @param[in] invocation
        ///     Method to invoke
        /// @param[in] interval
        ///     Interval between invocations
        /// @param[in] align
        ///     TimePoint alignment, i.e., when the first invocation takes place. Use
        ///       - \p ALIGN_START to invoke the task as soon as it added to the scheduler,
        ///       - \p ALIGN_NEXT to delay one cycle, until the specified interval has elapsed,
        ///       - \p ALIGN_UTC to align invocations to `interval` multiples from midnight, UTC
        ///         (technically, from midnight January 1, 1970, UTC, a.k.a. epoch).
        ///       - \p ALIGN_LOCAL to align invocations to midnight, local time (technically,
        ///         epoch minus the current local timezone offset).
        /// @param[in] loglevel
        ///     Verbosity at which task invocations are logged
        /// @param[in] count
        ///     Number of invocations; 0 is infinite.
        /// @param[in] retries
        ///     Maximum number of invocation failures to allow before unscheduling task.
        /// @param[in] catchup
        ///     Whether to immediately reschedule tasks that could not be executed in their
        ///     specified timeslot (because a prior invocation task took too long, or or due to
        ///     system load). Using this could lead to starvation.
        /// @return
        ///     Reference to the scheduled task
        Task &add(const Handle &handle,
                  const Invocation &invocation,
                  const dt::Duration &interval,
                  const Alignment align = ALIGN_NEXT,
                  status::Level loglevel = status::Level::DEBUG,
                  uint count = 0,
                  uint retries = 0,
                  bool catchup = false);

        /// @fn add_if_missing
        /// @brief Add a scheduled task if the specified ID does not already exist. \sa add
        Task &add_if_missing(const Handle &handle,
                             const Invocation &invocation,
                             const dt::Duration &interval,
                             const Alignment align = ALIGN_NEXT,
                             status::Level loglevel = status::Level::DEBUG,
                             uint count = 0,
                             uint retries = 0,
                             bool catchup = false);

        /// @brief Schedule a task/method to be invoked at specified time interval
        bool remove(const Handle &handle);
        bool remove(const Task &task);

        /// @fn exists
        /// @brief indicate whether the specifed task ID exists
        bool exists(const Handle &handle);

        /// @fn find
        /// @brief Find any scheduled task by the specified name
        /// @param[in] id
        ///     Task ID
        /// @return
        ///     Iterator to the task if found, or `this->end()` if not found.
        TaskMap::iterator find(const Handle &handle) noexcept;
        TaskMap::iterator begin() noexcept;
        TaskMap::iterator end() noexcept;

        /// @fn has_task
        /// @brief Check whether a specific task exists
        /// @param[in] id
        ///     Task ID
        /// @return
        ///     `true` if the task exists, `false` otherwise.
        bool has_task(const Handle &handle) const noexcept;

        /// @fn stop
        /// @brief stop the scheduler
        void stop();

    private:  // Methods
        dt::Duration local_offset(const dt::TimePoint &tp);
        void adjust_times(const dt::TimePoint &expected, const dt::TimePoint &now);
        Task &add_task(const dt::TimePoint &tp, Task &&task);
        bool remove_task(const Handle &handle, const Task *ptask = nullptr);
        void start_watcher();
        void stop_watcher();
        void watcher();

    private:  // Data
        dt::Duration max_nap;
        dt::Duration max_jitter;
        //dt::TimePoint timebase;
        TaskMap tasks;
        Task *current = nullptr;
        std::thread activeWatcher;
        std::mutex mtx;
        std::condition_variable stop_request;

    private:  // Classes
        /// @class Task
        /// @brief A scheduled task
        class Task
        {
            friend class Scheduler;

        private:
            Task(const Handle &handle,
                 const Invocation &invocation,
                 const dt::Duration &interval,
                 Alignment align,
                 uint count,
                 uint retries,
                 bool catchup,
                 status::Level loglevel);

            bool invoke(const dt::TimePoint &tp);
            dt::TimePoint aligned_time(const dt::TimePoint &now = dt::Clock::now()) const;
            dt::TimePoint next_time(const dt::TimePoint &tp,
                                    const dt::TimePoint &now = dt::Clock::now()) const;
            dt::TimePoint adjusted_time(const dt::TimePoint &old_time,
                                        const dt::TimePoint &new_time,
                                        const dt::TimePoint &tp) const;

            dt::TimePoint next_aligned(const dt::TimePoint &reference,
                                       const dt::TimePoint &now) const;

        public:
            Handle handle;
            Invocation invocation;
            dt::Duration interval;
            Alignment align;
            uint count, remaining;
            uint retries, failures;
            bool catchup;
            status::Level loglevel;
        };
    };

    extern Scheduler scheduler;
}  // namespace shared
