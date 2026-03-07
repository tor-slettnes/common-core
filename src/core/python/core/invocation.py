#!/usr/bin/python3
#===============================================================================
## @file invocation.py
## @brief Process related utilities
## @author Tor Slettnes
#===============================================================================

from typing import Callable, Coroutine, Sequence, Mapping, Optional

import sys
import traceback
import inspect
import logging
import asyncio
import multiprocessing

from .docbase import DocBase

LogFunc = Callable[[str], None]

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


def check_type(argument: object,
               expected_type: type):

    if not isinstance(argument, expected_type):
        raise TypeError('Expected %s, not %s (%s)'%(
            expected_type.__name__, type(argument).__name__, argument))


def safe_invoke(function    : Callable,
                args        : Sequence = (),
                kwargs      : Mapping = {},
                description : Optional[str] = None,
                log_call    : Optional[LogFunc] = logging.debug,
                log_failure : Optional[LogFunc] = logging.error,
                ) -> None:
    '''
    Invoke a callable (function) and catch exception
    '''

    if not description:
        description = invocation(function, args, kwargs)

    try:
        if log_call:
            log_call('Invoking %s'%(description,))

        return function(*args, **kwargs), None

    except Exception as e:
        log_invocation_failure(description, log_failure, e)
        return None, e


async def safe_await(function    : Callable,
                     args        : Sequence = (),
                     kwargs      : Mapping = {},
                     description : Optional[str] = None,
                     log_call    : Optional[LogFunc] = logging.debug,
                     log_failure : Optional[LogFunc] = logging.error,
                     ) -> None:
    '''
    Invoke a callable (function) and catch exception
    '''

    if not description:
        description = invocation(function, args, kwargs)

    try:
        if log_call:
            log_call('Invoking %s'%(description,))

        result = function(*args, **kwargs)
        if asyncio.iscoroutine(result):
            result = await result

        return result, None

    except Exception as e:
        log_invocation_failure(description, log_failure, e)
        return None, e


def log_invocation_failure(description : Optional[str],
                           log_func    : Optional[LogFunc],
                           exception   : Exception):

    if log_func:
        log_func(
            "Exception occured in %s: [%s] %s"%(
                description,
                type(exception).__name__,
                exception,
            ),
            exc_info = exception,
        )


def invocation(
        method: Callable,
        args: Sequence = (),
        kwargs: Mapping = {}):
    arglist  = [ "%r"%(arg,) for arg in args ]
    arglist += [ "%s=%r"%item for item in kwargs.items() ]
    return "%s(%s)"%(method.__name__, ", ".join(arglist))


def caller_frame(hops: int = 1):
    stack = inspect.stack()
    return stack[hops+1]


def process_name() -> str:
    '''
    Return the name of the currently runningn process
    '''
    return multiprocessing.current_process().name


def main_module_path() -> str:
    '''
    Return the fully qualified name of the main module
    '''
    try:
        return sys.modules['__main__'].__spec__.name
    except (KeyError, AttributeError):
        return ''


def method_path(method: Callable) -> str:
    '''
    Obtain code context (module, qualified name) of the provided method.
    '''

    return '.'.join((method.__module__, method.__qualname__))



def invoke_async(
        function: Callable,
        args: Sequence = (),
        kwargs: Mapping = {}):
    '''
    Invoke an AsyncIO coroutine as a new task.  A reference is kept to this
    task in order to prevent it from disappearing mid-execution; see
    <https://docs.python.org/3/library/asyncio-task.html#creating-tasks>.
    '''
    async_tasks.add_coroutine(function(*args, **kwargs))


def invoke_maybe_async(
        function: Callable,
        args: Sequence = (),
        kwargs: Mapping = {}):
    '''
    Invoke a function that may or may not be an AsyncIO coroutine.

    If invoking the function with the specified arguments returns a coroutine
    object, it is lauched in a new asyncio task. A reference is kept to this
    task in order to prevent it from disappearing mid-execution; see
    <https://docs.python.org/3/library/asyncio-task.html#creating-tasks>.
    '''

    result = function(*args, **kwargs)
    if asyncio.iscoroutine(result):
        async_tasks.add_coroutine(result)
    else:
        return result


def safe_invoke_maybe_async(
        function      : Callable,
        args        : Sequence = (),
        kwargs      : Mapping = {},
        description : Optional[str] = None,
        log_call    : Optional[Callable[[str], None]] = logging.debug,
        log_failure : Optional[Callable[[str], None]] = logging.error) -> None:
    '''
    Invoke a possibly-asynchrnonous callable (function) and catch exception.

    Any exceptions raised invoking the function are logged, but not propagated
    back to the caller.

    If invoking the function with the specified arguments returns a coroutine
    object, it is lauched in a new asyncio task. A reference is kept to this
    task in order to prevent it from disappearing mid-execution; see
    <https://docs.python.org/3/library/asyncio-task.html#creating-tasks>.
    '''

    return safe_invoke(
        function = invoke_maybe_async,
        kwargs = dict(function=function, args=args, kwargs=kwargs),
        description = description,
        log_call = log_call,
        log_failure = log_failure)
