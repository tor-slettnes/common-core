#!/usr/bin/python3
#===============================================================================
## @file safe_invoke.py
## @brief Process related utilities
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from typing  import Callable, Sequence, Mapping, Optional

import sys
import traceback
import logging

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


def invocation(method, args, kwargs):
    arglist  = [ "%r"%(arg,) for arg in args ]
    arglist += [ "%s=%r"%item for item in kwargs.items() ]
    return "%s(%s)"%(method.__name__, ", ".join(arglist))
