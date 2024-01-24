/// -*- c++ -*-
//==============================================================================
/// @file scheduler.c++
/// @brief schedule tasks to be invoked at specified intervals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "scheduler.h++"
#include "date-time.h++"
#include "string/format.h++"
#include "status/exceptions.h++"

namespace cc
{

    //==========================================================================
    // Scheduler methods

    Scheduler::Scheduler(dt::Duration max_nap,
                         dt::Duration max_jitter)
        : max_nap(max_nap),
          max_jitter(max_jitter)
    {
    }

    Scheduler::~Scheduler()
    {
        this->stop();
    }

    Scheduler::Task &Scheduler::add(const std::string &id,
                                    const Invocation &invocation,
                                    const dt::Duration &interval,
                                    const Alignment align,
                                    status::Level loglevel,
                                    uint count,
                                    uint retries,
                                    bool catchup)
    {
        if (interval <= dt::Duration::zero())
        {
            throw exception::InvalidArgument("A positive interval is required", interval);
        }

        auto lck = std::lock_guard(this->mtx);
        dt::TimePoint now = dt::Clock::now();
        //Task task(id, invocation, interval, align, count, retries, catchup);
        Task task(id, invocation, interval, align, count, retries, catchup, loglevel);
        dt::TimePoint tp = task.aligned_time(now);
        return this->add_task(tp, std::move(task));
    }

    Scheduler::Task &Scheduler::add_if_missing(const std::string &id,
                                               const Invocation &invocation,
                                               const dt::Duration &interval,
                                               const Alignment align,
                                               status::Level loglevel,
                                               uint count,
                                               uint retries,
                                               bool catchup)
    {
        auto it = this->find(id);
        if (it == this->end())
        {
            return this->add(id, invocation, interval, align, loglevel, count, retries, catchup);
        }
        else
        {
            return it->second;
        }
    }

    bool Scheduler::remove(const Scheduler::Task &task)
    {
        auto lck = std::lock_guard(this->mtx);
        return this->remove_task({}, &task);
    }

    bool Scheduler::remove(const std::string &id)
    {
        auto lck = std::lock_guard(this->mtx);
        return this->remove_task(id);
    }

    bool Scheduler::exists(const std::string &id)
    {
        return this->find(id) != this->end();
    }

    Scheduler::TaskMap::iterator Scheduler::find(const std::string &id) noexcept
    {
        auto lck = std::lock_guard(this->mtx);
        for (auto it = this->begin(); it != this->end(); it++)
        {
            if (it->second.id == id)
            {
                return it;
            }
        }
        return this->end();
    }

    Scheduler::TaskMap::iterator Scheduler::begin() noexcept
    {
        return this->tasks.begin();
    }

    Scheduler::TaskMap::iterator Scheduler::end() noexcept
    {
        return this->tasks.end();
    }

    bool Scheduler::has_task(const std::string &id) const noexcept
    {
        for (const auto &[tp, task] : this->tasks)
        {
            if (task.id == id)
            {
                return true;
            }
        }
        return false;
    }

    void Scheduler::stop()
    {
        auto lck = std::lock_guard(this->mtx);
        this->stop_watcher();
    }

    Scheduler::Task &Scheduler::add_task(const dt::TimePoint &tp, Scheduler::Task &&task)
    {
        // Remove any existing task with the same ID
        this->remove_task(task.id);

        // Add the new task to the schedule.
        auto it = this->tasks.emplace(tp, task);
        bool isfirst = (it == this->tasks.begin());
        logf_trace("Added task: id=%r, next=%r, first=%r", task.id, tp, isfirst);

        // Was this task inserted at the beginning?
        if (isfirst)
        {
            // If so, start a new watcher thread. To do so, we must first exit
            // any existing thread.
            this->stop_watcher();

            // Start a new watcher thread.
            logf_trace("Creating watcher thread with initial task %r", it->second.id);
            this->start_watcher();
        }

        return it->second;
    }

    void Scheduler::start_watcher()
    {
        this->activeWatcher = std::thread(&Scheduler::watcher, this);
        logf_debug("Started watcher thread");
    }

    void Scheduler::stop_watcher()
    {
        std::thread watcher = std::move(this->activeWatcher);
        if (watcher.joinable())
        {
            this->mtx.unlock();
            this->stop_request.notify_all();
            watcher.join();
            this->mtx.lock();
            logf_debug("Stopped watcher thread");
        }
    }

    bool Scheduler::remove_task(const std::string &id, const Task *ptask)
    {
        bool found = false;
        for (auto it = this->tasks.begin(); it != this->tasks.end();)
        {
            Task &task = it->second;
            if ((ptask == nullptr) ? (task.id == id) : (&task == ptask))
            {
                bool isfirst = (&task == this->current);
                logf_trace("Removing task: id=%r, next=%s, first=%s", id, it->first, isfirst);
                if (isfirst)
                {
                    this->current = nullptr;
                }
                it = this->tasks.erase(it);
                found = true;
            }
            else
            {
                it++;
            }
        }
        logf_trace("remove_task: id=%r, found=%r", id, found);
        return found;
    }

    void Scheduler::watcher()
    {
        std::unique_lock lck(this->mtx);
        dt::TimePoint now = dt::Clock::now();
        logf_trace("Starting watcher thread at %s", now);
        while (this->tasks.size())
        {
            auto it = this->tasks.begin();
            auto &[tp, task] = *it;

            std::string id = task.id;
            this->current = &task;
            steady::TimePoint deadline = steady::Clock::now() + std::min(this->max_nap, (tp - now));

            this->stop_request.wait_until(
                lck, deadline, [&] {
                    return !this->activeWatcher.joinable();
                });

            if (!this->activeWatcher.joinable())
            {
                logf_debug(
                    "Watcher thread was cancelled while waiting for task %r; exiting.",
                    id);
                break;
            }

            now = dt::Clock::now();

            if (this->current != nullptr)
            {
                if ((now < tp) || (now > tp + this->max_jitter))
                {
                    // The system clock shifted.  Adjust invocation timestamps.
                    this->adjust_times(tp, now);
                }
                else if (this->current == &task)
                {
                    this->mtx.unlock();
                    bool keep = task.invoke(tp);
                    this->mtx.lock();
                    now = dt::Clock::now();
                    auto nh = this->tasks.extract(it);

                    if (keep && this->current)
                    {
                        nh.key() = nh.mapped().next_time(tp, now);
                        //logf_trace("Scheduled task %r next invocation at %r", id, nh.key());
                        this->tasks.insert(std::move(nh));
                    }
                    else
                    {
                        logf_debug("Scheduled task %r ended", id);
                    }
                }
                else
                {
                    logf_trace("Scheduled task %r was removed, moving on", id);
                }
            }
        }
        logf_trace("Ending watcher thread at %s", now);
    }

    void Scheduler::adjust_times(const dt::TimePoint &expected,
                                 const dt::TimePoint &now)
    {
        logf_notice(
            "Clock skew detected (expected to wake up at %s, but now it's %s). "
            "Shifting time reference for %d tasks by %s.",
            expected,
            now,
            this->tasks.size(),
            now - expected);

        TaskMap update;
        while (this->tasks.size())
        {
            auto nh = this->tasks.extract(this->tasks.begin());
            nh.key() = nh.mapped().adjusted_time(expected, now, nh.key());
            update.insert(std::move(nh));
        }
        std::swap(this->tasks, update);
    }

    //==========================================================================
    // Task methods

    Scheduler::Task::Task(const std::string &id,
                          const Invocation &invocation,
                          const dt::Duration &interval,
                          Alignment align,
                          uint count,
                          uint retries,
                          bool catchup,
                          status::Level loglevel)
        : id(id),
          invocation(invocation),
          interval(interval),
          align(align),
          count(count),
          remaining(count),
          retries(retries),
          failures(0),
          catchup(catchup),
          loglevel(loglevel)
    {
    }

    bool Scheduler::Task::invoke(const dt::TimePoint &tp)
    {
        bool keep = false;

        try
        {
            const Invocation &f = this->invocation;
            logf_message(this->loglevel,
                  "Scheduled task %r invocation (variant %d)",
                  this->id,
                  f.index());
            switch (f.index())
            {
            case 0:
                std::get<std::function<
                    void()>>(f)();
                break;

            case 1:
                std::get<std::function<
                    void(const dt::TimePoint &)>>(f)(tp);
                break;

            case 2:
                std::get<std::function<
                    void(const dt::TimePoint &, const Task &)>>(f)(tp, *this);
                break;
            }

            keep = ((this->count == 0) || ((this->remaining > 0) && (--this->remaining > 0)));
        }
        catch (...)
        {
            keep = (this->failures < this->retries);

            if (keep)
            {
                this->failures++;
                logf_notice("Scheduled task %r invocation failed, %d tries remaining: %s",
                            this->id,
                            this->retries - this->failures + 1,
                            std::current_exception());
            }
            else
            {
                logf_notice("Scheduled task %r invocation failed %d times, stopping: %s",
                            this->id,
                            this->failures,
                            std::current_exception());
            }
        }

        return keep;
    }

    dt::TimePoint Scheduler::Task::aligned_time(const dt::TimePoint &now) const
    {
        switch (this->align)
        {
        case ALIGN_START:
            return now;

        case ALIGN_NEXT:
            return now + this->interval;

        case ALIGN_UTC:
            return this->next_aligned(dt::epoch, now);

        case ALIGN_LOCAL:
            return this->next_aligned(dt::epoch + dt::local_adjustment(now), now);
        default:
            return dt::epoch;
        }
    }

    dt::TimePoint Scheduler::Task::next_time(const dt::TimePoint &tp,
                                             const dt::TimePoint &now) const
    {
        // By default, advance by the specified interval
        dt::TimePoint next = tp + this->interval;

        // If we're aligning to local time, and our interval does not evenly divide an hour
        // (e.g. 4 hours) adjust for any change in the local timezone offset (i.e.,
        // entering/leaving DST).
        if ((this->align == ALIGN_LOCAL) &&
            (std::chrono::hours(1) % this->interval != dt::Duration::zero()))
        {
            next += (dt::local_adjustment(next) - dt::local_adjustment(tp));
        }

        // If the next scheduled time has already passed, and unless we're in catchup mode,
        // advance to the next possible time slot.
        if ((next < now) && !this->catchup)
        {
            next = now - ((now - next) % this->interval) + this->interval;
        }

        return next;
    }

    dt::TimePoint Scheduler::Task::adjusted_time(const dt::TimePoint &old_time,
                                                 const dt::TimePoint &new_time,
                                                 const dt::TimePoint &tp) const
    {
        switch (this->align)
        {
        case ALIGN_START:
        case ALIGN_NEXT:
            return tp + (new_time - old_time);

        case ALIGN_LOCAL:
        case ALIGN_UTC:
            return this->aligned_time(new_time);

        default:
            return dt::epoch;
        }
    }

    dt::TimePoint Scheduler::Task::next_aligned(const dt::TimePoint &reference,
                                                const dt::TimePoint &tp) const
    {
        dt::TimePoint aligned = dt::last_aligned(tp, reference, this->interval);
        if (aligned < tp)
        {
            aligned += this->interval;
        }
        return aligned;
    }

    //==========================================================================
    // Global instance

    Scheduler scheduler;
}  // namespace cc
