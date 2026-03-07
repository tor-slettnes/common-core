'''
invocation.py - safely invoke unprotected methods
'''

from typing import Callable, Coroutine, Sequence, Mapping, Optional

import sys
import traceback
import inspect
import logging
import asyncio
import multiprocessing

from .asynctask import async_tasks

LogFunc = Callable[[str], None]

def safe_invoke(function    : Callable,
                args        : Sequence = (),
                kwargs      : Mapping = {},
                description : Optional[str] = None,
                log_call    : Optional[LogFunc] = logging.debug,
                log_failure : Optional[LogFunc] = logging.error,
                ):
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
                     ):
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


def safe_invoke_maybe_async(
        function      : Callable,
        args        : Sequence = (),
        kwargs      : Mapping = {},
        description : Optional[str] = None,
        log_call    : Optional[Callable[[str], None]] = logging.debug,
        log_failure : Optional[Callable[[str], None]] = logging.error):
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
        kwargs: Mapping = {}) -> str:
    arglist  = [ "%r"%(arg,) for arg in args ]
    arglist += [ "%s=%r"%item for item in kwargs.items() ]
    return "%s(%s)"%(method.__name__, ", ".join(arglist))


