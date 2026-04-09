'''
asynctask.py - AsyncIO task manager
'''

__author__ = 'Tor Slettnes'

import asyncio
import logging
from typing import Coroutine, Callable


class AsyncTasks (set):
    '''
    Collection of asynchronous tasks. A reference is kept to tasks in order
    to prevent them from disappearing mid-execution; see
    <https://docs.python.org/3/library/asyncio-task.html#creating-tasks>.
    '''

    def add_coroutine(self,
                      coroutine: Coroutine,
                      log_failure: Callable = logging.exception,
                      ) -> asyncio.Task:
        '''
        Add a task to run specified coroutine in the background.

        Example:

        ```python
        tasks = AsyncTasks()
        tasks.add_coroutine(asyncio.sleep(10))
        ```
        '''
        return self.add(
            asyncio.create_task(coroutine, name=coroutine.__qualname__),
            log_failure,
        )

    def add(self,
            task: asyncio.Task,
            log_failure: Callable = logging.exception):
        '''
        Add an existing task.

        Example:

        ```python
        task = asyncio.create_task(asyncio.sleep(10))

        tasks = AsyncTasks()
        tasks.add(task)
        ```
        '''

        task.add_done_callback(lambda task: self.on_done(task, log_failure))
        if task.get_name().startswith("Task-"):
            task.set_name(task.get_coro().__qualname__)

        set.add(self, task)
        return task


    def append(self, task: asyncio.Task):
        '''
        Synonym for `add()`, for compatibility with existing applications.
        '''
        self.add(task)


    def on_done(self,
                task: asyncio.Task,
                log_failure: Callable = logging.exception,
                ):
        '''
        Log any exceptions from the task, and remove its reference
        '''

        self.discard(task)

        try:
            if exception := task.exception():
                raise task.exception()

        except (SystemExit, asyncio.CancelledError):
            pass

        except Exception as e:
            if log_failure:
                log_failure(
                    "Task %r failed: [%s] %s"%(
                        task.get_name(),
                        type(e).__name__,
                        e,
                ))


    def cancel_all(self):
        '''
        Cancel all managed AsycncIO tasks.
        '''
        for task in self:
            task.cancel()


async_tasks = AsyncTasks()

