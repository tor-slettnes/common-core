'''
Miscellaneous function decorators
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

from typing import Callable

#-------------------------------------------------------------------------------
# @override - indicate overriden function

try:
    ### Python 3.12 or above already has @override
    from typing import override
except ImportError:
    ### For Python 3.11 or earlier, we create a dummy
    def override(function: Callable):
        return function

#-------------------------------------------------------------------------------
# @doc_inherit - Inherit docstrings from parent class
#
# Copied ad verbatim from:
# https://sqlpey.com/python/solved-how-to-inherit-docstrings-in-python-class-inheritance/

from functools import wraps

class DocInherit(object):
    """
    A method descriptor that inherits docstrings from parent classes.
    """

    def __init__(self, method):
        self.method = method
        self.name = method.__name__

    def __get__(self, instance, owner):
        if instance:
            return self._get_with_instance(instance, owner)
        return self._get_no_instance(owner)

    def _get_with_instance(self, instance, owner):
        overridden = getattr(super(owner, instance), self.name, None)

        @wraps(self.method, assigned=('__name__', '__module__'))
        def wrapper(*args, **kwargs):
            return self.method(instance, *args, **kwargs)

        return self._use_parent_doc(wrapper, overridden)

    def _get_no_instance(self, owner):
        for parent in owner.__mro__[1:]:
            overridden = getattr(parent, self.name, None)
            if overridden:
                break

        @wraps(self.method, assigned=('__name__', '__module__'))
        def wrapper(*args, **kwargs):
            return self.method(*args, **kwargs)

        return self._use_parent_doc(wrapper, overridden)

    def _use_parent_doc(self, func, source):
        if source is None:
            raise NameError(f"Could not find '{self.name}' in parents")
        func.__doc__ = source.__doc__
        return func

doc_inherit = DocInherit


#-------------------------------------------------------------------------------
# @typecheck - ensure input arguemnts and return value match provided type hints

from inspect import getfullargspec, get_annotations

def typecheck(function: Callable) -> Callable:
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

    annotations     = get_annotations(function)
    positionals     = [annotations.get(arg_name, object)
                       for arg_name in getfullargspec(function).args]
    expected_return = annotations.pop('return', object)


    def checktype(encountered: object, expected: type) -> bool:
        if issubclass(complex, expected):
            expected |= float

        if issubclass(float, expected):
            expected |= int

        return isinstance(encountered, expected)


    def verify(encountered: object, expected: type, arg: str|int):
        if not checktype(encountered, expected):
            raise TypeError(
                f"Expected {function.__name__}() {arg} of type {expected}, "
                f"got {type(encountered).__name__}")


    def wrapper(*args, **kwargs) -> object:
        '''
        Check provided input arguments against type hints, invoke method,
        then check return value against type hint.
        '''

        for i, arg in enumerate(args):
            try:
                expected_type = positionals[i]
            except IndexError:
                break
            else:
                verify(arg, expected_type, f"argument {i}")

        for key, value in kwargs.items():
            try:
                expected_type = annotations[key]
            except KeyError:
                pass
            else:
                verify(value, expected_type, f"argument {key!r}")

        result = function(*args, **kwargs)
        verify(result, expected_return, "return value")

        return result

    return wrapper
