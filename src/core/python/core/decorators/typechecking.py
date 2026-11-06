'''
typecheck decorator - ensure input argumetns and return value match provided type hints.
'''

__author__ = 'Tor Slettnes'

from inspect import getfullargspec
from functools import wraps
from typing import Callable

def typecheck(function: Callable):
    '''
    Decorator to ensure input arguments and return value match provided type hints.

    Example:

      ```python

      >>> @typecheck
      ... def my_func(key: str, target: float, tolerance: float) -> dict[str, tuple[float, float]]:
              return {key: (target - abs(tolerance),  target + abs(tolerance))}

      >>> my_func("temperature_target", 60, 0.5)
      {"setpoint": (59.5, 60.5)}

      >>> my_func("pressure_target", 400, None)
      TypeError: Expected my_func() tolerance of type float, got NoneType
      ```
    '''

    @wraps(function)
    def wrapper(*args, **kwargs) -> object:
        '''
        Check provided input arguments against type hints, invoke method,
        then check return value against type hint.
        '''

        argspec = getfullargspec(function)

        for i, arg in enumerate(args):
            try:
                arg_name = argspec.args[i]
                expected_type = argspec.annotations[arg_name]
            except IndexError:
                break
            except KeyError:
                pass
            else:
                verify(function, arg, expected_type, f"argument {arg_name!r}")

        for key, value in kwargs.items():
            try:
                expected_type = argspec.annotations[key]
            except KeyError:
                pass
            else:
                verify(function, value, expected_type, f"argument {key!r}")

        result = function(*args, **kwargs)

        if expected_return := argspec.annotations.get('return'):
            verify(function, result, expected_return, "return value")

        return result

    return wrapper


def verify(function: Callable,
           encountered: object,
           expected: type,
           item_description: str):

    if not checktype(encountered, expected):
        raise TypeError(
            "Expected %s() %s of type %s, got %s: %r"%(
                function.__name__,
                item_description,
                expected.__name__,
                type(encountered).__name__,
                encountered,
            ))


def checktype(encountered: object,
              expected: type) -> bool:
    if issubclass(complex, expected):
        expected |= float

    if issubclass(float, expected):
        expected |= int

    return isinstance(encountered, expected)
