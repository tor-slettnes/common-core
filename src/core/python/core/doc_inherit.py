#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file doc_inherit.py
## @brief Decorator for instance methods to inherit docstring frm parent
## @author Tor Slettnes <tor@slett.net>
##
## Copied ad verbatim from:
## https://sqlpey.com/python/solved-how-to-inherit-docstrings-in-python-class-inheritance/
#===============================================================================

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
