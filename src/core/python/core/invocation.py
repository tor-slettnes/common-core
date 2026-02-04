#!/usr/bin/python3
#===============================================================================
## @file invocation.py
## @brief Process related utilities
## @author Tor Slettnes
#===============================================================================

from typing import Callable, Coroutine, Sequence, Mapping, Optional, Any

import sys
import traceback
import inspect
import logging
import asyncio

def check_type(argument: object,
               expected_type: type):

    if not isinstance(argument, expected_type):
        raise TypeError('Expected %s, not %s (%s)'%(
            expected_type.__name__, type(argument).__name__, argument))


def safe_invoke(function    : Callable,
                args        : Sequence = (),
                kwargs      : Mapping = {},
                description : Optional[str] = None,
                log_call    : Optional[Callable[[str], None]] = logging.debug,
                log_failure : Optional[Callable[[str], None]] = logging.error) -> None:
    '''
    Invoke a callable (function) and catch exception
    '''
    try:
        if log_call:
            log_call('Invoking %s'%(
                description or invocation(function, args, kwargs),
            ))

        return function(*args, **kwargs)

    except Exception as e:
        e_type, e_name, e_tb = sys.exc_info()
        if log_failure:
            log_failure("Exception occured in %s:\n%s\n[%s] %s"%(
                description or invocation(function, args, kwargs),
                stack_trace(tb=e_tb),
                type(e).__name__,
                e))

        ## Prevent circular reference, per https://docs.python.org/2/library/sys.html.
        del e_tb
        return e

def stack_trace(tb=None):
    msg = []
    if tb:
        msg.extend(["  In %s, method %s(), line %d: %s\n"%(filepath, method, lineno, text)
                    for filepath, lineno, method, text in traceback.extract_tb(tb)])
    return "".join(msg)


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


def method_path(method: Callable) -> str:
    '''
    Obtain code context (module, class, method name) of the provided
    instance method.
    '''
    return '.'.join((
        method.__module__,
        type(method.__self__).__name__,
        interceptor.__name__,
    ))


class AsyncTasks (set):
    '''
    Collection of asynchronous tasks. A reference is kept to tasks in order
    to prevent them from disappearing mid-execution; see
    <https://docs.python.org/3/library/asyncio-task.html#creating-tasks>.
    '''

    def add_coroutine(self, coroutine: Coroutine):
        self.add(asyncio.create_task(coroutine))

    def add(self, task: asyncio.Task):
        task.add_done_callback(self.discard)
        set.add(self, task)


async_tasks = AsyncTasks()


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

    safe_invoke(
        function = invoke_maybe_async,
        kwargs = dict(function=function, args=args, kwargs=kwargs),
        description = description,
        log_call = log_call,
        log_failure = log_failure)
