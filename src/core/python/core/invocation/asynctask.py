'''
asynctask.py - AsyncIO task manager
'''

__author__ = 'Tor Slettnes'

import asyncio
from typing import Coroutine
from ..docbase import DocBase

class AsyncTasks (DocBase, set):
    '''
    Collection of asynchronous tasks. A reference is kept to tasks in order
    to prevent them from disappearing mid-execution; see
    <https://docs.python.org/3/library/asyncio-task.html#creating-tasks>.
    '''

    def add_coroutine(self, coroutine: Coroutine):
        '''
        Add a task to run specified coroutine in the background.

        Example:

        ```python
        tasks = AsyncTasks()
        tasks.add_coroutine(asyncio.sleep(10))
        ```
        '''
        return self.add(
            asyncio.create_task(
                coroutine,
                name=coroutine.__qualname__))

    def add(self, task: asyncio.Task):
        '''
        Add an existing task.

        Example:

        ```python
        task = asyncio.create_task(asyncio.sleep(10))

        tasks = AsyncTasks()
        tasks.add(task)
        ```
        '''

        task.add_done_callback(self.discard)
        if task.get_name().startswith("Task-"):
            task.set_name(task.get_coro().__qualname__)

        set.add(self, task)
        return task


    def append(self, task: asyncio.Task):
        '''
        Synonym for `add()`, for compatibility with existing applications.
        '''
        self.add(task)


    def cancel_all(self):
        '''
        Cancel all managed AsycncIO tasks.
        '''
        for task in self:
            task.cancel()


async_tasks = AsyncTasks()

