#!/usr/bin/python3 -i
#===============================================================================
## @file scheduler.py
## @brief Schedule callbacks at regular intervals
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import time, os, sys, traceback

from threading import Thread, Lock, Condition, Event, current_thread, ThreadError
from logging   import debug, info, warning, error
from typing    import Callable, Optional

try:
    # Python 3 has math.inf, but does not allow sorting between numeric types and strings
    from math import inf as INFINITY
except ImportError:
    # Python 2 does not have math.inf, but considers any number < any string.
    INFINITY = "infinity"

TaskAlignment = ('none', 'local', 'utc')
ALIGN_NONE, ALIGN_LOCAL, ALIGN_UTC = range(len(TaskAlignment))

class Scheduler (object):

    def __init__(self, identity : str):
        ## Public data members
        self.speedFactor = 1.0

        ## Private data members
        self._identity         = identity
        self._nextcount        = 0
        self._tick             = 0.001
        self._maxsleep         = 0.1
        self._timeshiftlimit   = 5.0
        self._retries          = {}
        self._mutex            = Lock()
        self._running          = True
        self._starttime        = 0
        self._timerThread      = None

        self._locks            = { INFINITY:[] }
        self._pendingStart     = {}
        self._pendingException = {}
        self._tasks            = {}


    def start(self):
        self._running = True
        if not self._timerThread:
            info('Starting task scheduler')
            self._timerThread = Thread(None, self._timerloop, name=self._identity, args=(), daemon=True)
            self._timerThread.start()


    def shutdown(self):
        info('Shutting down task scheduler')
        self._running = False
        if thread := self._timerThread:
            thread.join()
            self._timerThread = None


    def alignmentOffset(self, interval, align=ALIGN_LOCAL, timestamp=None):
        if timestamp is None:
            timestamp = time.time()

        if align == ALIGN_UTC:
            tz  = 0
        elif align == ALIGN_LOCAL:
            dst = time.localtime(timestamp)[-1]
            tz  = (time.timezone, time.altzone)[dst]
        else:
            tz = self._starttime

        return int(timestamp - ((timestamp - tz) % interval))

    def setalarm(self, timeout, lock, suspendable=False):
        with _mutex:
            self._setalarm(timeout, lock, suspendable)

    def _setalarm(self, timeout, lock, suspendable=False):
        count = _nextcount + max(0, self._tickCount(timeout))
        self._locks.setdefault(count, []).append(lock)

    def clearalarm(self, lock):
        with self._mutex:
            return self._clearalarm(lock)

    def _clearalarm(self, lock):
        for tasklist in self._locks.values():
            try:
                tasklist.remove(lock)
                return True
            except ValueError:
                pass

        return False


    def scheduleList(self):
        return [name for name, lock, thread in self._tasks.values()]

    def getTask(self, handle):
        name, lock, thread = self._tasks[handle.lower()]
        return thread._Thread__args

    def isStarted(self, handle):
        name, lock, thread = self._tasks[handle.lower()]
        return thread._Thread__started


    def reschedule(self, handle, delay=0):
        with self._mutex:
            name, lock, thread = self._tasks.pop(handle.lower())
            self._clearalarm(lock)
            self._setalarm(delay, lock)


    def schedule(self,
                 handle     : str,
                 interval   : float,
                 method     : callable,
                 args       : tuple = (),
                 kwargs     : dict  = {},
                 count      : Optional[int] = None,
                 retries    : int = 0,
                 align      : int = ALIGN_NONE,
                 delay      : int = 0,
                 synchronous: bool = False,
                 waitstart  : bool = False):

        args = (handle, interval, method, args, kwargs, count, retries, align, delay)

        ### Acquire task scheduling mutex; this will be released within
        ### the "_runTask" method.
        self._mutex.acquire()

        if (synchronous):
            self._runTask(*args)
        else:
            if waitstart:
                pending = self._pendingStart[handle.lower()] = Event()

            thread = Thread(None, self._runTask, handle, args, daemon=True)
            thread.start()

            if waitstart:
                pending.wait()
                exception = self._pendingException.get(handle, None)

                if exception:
                    exc, tb = exception
                    raise exc.with_traceback(tb) from None



    def unschedule(self, handle, wait=True):
        with self._mutex:
            try:
                name, lock, thread = self._tasks.pop(handle.lower())
            except KeyError:
                found = False
            else:
                self._clearalarm(lock)

                try:
                    lock.release()
                except ThreadError:
                    pass

                found = True

        ### Wait for schedule thread to complete before returning.
        if wait and found and (thread is not current_thread()):
            thread.join()

        return found


    def _runTask(self, handle, interval, method, args, kwargs, repeats, maxretries, align, delay):
        try:
            frequency  = max(1, self._tickCount(interval))
            delaycount = self._tickCount(delay)
            count      = 0
            failures   = 0
            lock       = Lock()
            lock.acquire()

            try:
                otherName, otherLock, otherThread = self._tasks[handle.lower()]
            except KeyError:
                otherLock = None

            self._tasks[handle.lower()] = thisHandle = handle, lock, current_thread()

            if otherLock:
                otherLock.release()

            nextslot = self._nextcount + delaycount
            nexttime = self._starttime + (self._tick * nextslot)

            if align:
                aligntime  = self.alignmentOffset(interval, align=align, timestamp=nexttime)
                nexttime   = aligntime + interval
                nextslot   = int((nexttime - self._starttime) / self._tick + 1)

            self._locks.setdefault(nextslot, []).append(lock)

            debug("Scheduling task %r to run every %s seconds, starting at %s (local time %s)"%
                  (handle, interval, nexttime,
                   time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(nexttime))))

        finally:
            self._mutex.release()


        pending = self._pendingStart.pop(handle.lower(), None)

        self.start()

        while (repeats == None) or (repeats > 0):
            ### Acquire the timing lock. This will be released by self._timerloop()
            lock.acquire()

            ### If we are no longer in the task list, we got unscheduled
            if self._tasks.get(handle.lower()) is not thisHandle:
                break

            else:
                try:
                    debug("Invoking scheduled task %r"%(handle,))
                    method(*args, **kwargs)
                    failures = 0
                    count += 1

                except Exception as e:
                    e_type, e_name, e_tb = sys.exc_info()
                    failures += 1

                    if failures > maxretries:
                        if pending:
                            self._pendingException[handle] = (e, e_tb)
                        else:
                            warning("Scheduled task %r failed %d times, stopping: [%s]: %s"%
                                    (handle, failures, type(e).__name__, e))
                            debug(self._stack_trace(e_tb))
                        break
                    else:
                        info("Scheduled task %r failed, %d retries remaining: [%s]"%
                             (handle, maxretries-failures+1, type(e).__name__))
                finally:
                    if pending:
                        pending.set()
                        pending = None

                if repeats:
                    repeats -= 1

            with self._mutex:
                if nextslot < self._nextcount:
                    nextslot = self._nextcount-1+frequency - (self._nextcount-1-nextslot)%frequency

                self._locks.setdefault(nextslot, []).append(lock)


        debug("Unscheduling task %r"%(handle,))

        if pending:
            pending.release()

        with self._mutex:
            if self._tasks.get(handle.lower()) is thisHandle:
                del self._tasks[handle.lower()]


    def _stack_trace(self, tb):
        msg = ["  In %s, method %s(), line %d: %s\n"%(filepath, method, lineno, text)
               for filepath, lineno, method, text in traceback.extract_tb(tb)]
        return "".join(msg)

    def _tickCount(self, time):
        return int(time / self.speedFactor / self._tick)

    def _timerloop(self):
        self._starttime = nexttime = now = time.time()

        while self._running:
            try:
                with self._mutex:
                    for lock in self._locks.pop(self._nextcount, []):
                        try:
                            lock.release()
                        except ThreadError:
                            pass

                    now = time.time()

                    if abs(now - nexttime) > self._timeshiftlimit:
                        shift = now - nexttime
                        warning("Clock skew detected. "
                                "Shifting time reference %.1f seconds (from %.1f to %.1f)"%
                                (shift, self._starttime, self._starttime+shift))
                        self._starttime += shift

                    maxticks   = self._tickCount(now + self._maxsleep - self._starttime)
                    self._nextcount = min(min(self._locks), maxticks)
                    nexttime   = self._starttime + (self._tick * self._nextcount)

                if (nexttime > now):
                    time.sleep(nexttime - now)

            except Exception as e:
                error("Internal scheduler error: %s"%e)


scheduler = Scheduler('Main scheduler')

