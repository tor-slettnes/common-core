#!/usr/bin/python3
#===============================================================================
## @file safe_invoke.py
## @brief Process related utilities
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from typing  import Callable, Sequence, Mapping, Optional

import sys
import traceback

def safe_invoke(function    : Callable,
                args        : Sequence = (),
                kwargs      : Mapping = {},
                description : Optional[str] = None,
                log         : Callable[[str], None] = sys.stderr.write) -> None:
    '''
    Invoke a callable (function) and catch exception
    '''
    try:
        return function(*args, **kwargs)

    except Exception as e:
        e_type, e_name, e_tb = sys.exc_info()
        log("Exception occured in %s:\n%s\n[%s] %s"%
            ((description or 'invocation '+invocation(function, args, kwargs)),
             stack_trace(tb=e_tb),
             type(e).__name__,
             e))
        del e_tb  ## Prevent circular reference, per https://docs.python.org/2/library/sys.html.
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
