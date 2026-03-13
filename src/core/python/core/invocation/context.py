'''
introspection.py - Examine code context
'''

__author__ = 'Tor Slettnes'

from typing import Callable

import sys
import inspect
import multiprocessing

def check_type(argument: object,
               expected_type: type):
    '''
    Ensure the provided `argument` is an instance of `expected_type`.
    Raises a `TypeError` otherwise.
    '''

    if not isinstance(argument, expected_type):
        raise TypeError(
            'Expected %s, not %s (%s)'%(
                expected_type.__name__,
                type(argument).__name__,
                argument,
            )
        )


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


def caller_frame(hops: int = 1):
    stack = inspect.stack()
    return stack[hops+1]


