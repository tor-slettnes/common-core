__author__ = 'Tor Slettnes'

class DocBase:
    '''
    Mix-in class that casues overridden methods of derived classes to
    inherit docstrings from their parent.
    '''
    __docstrings__ = {}

    def __init_subclass__(subclass, **kwargs):
        subclass.__docstrings__ = subclass.__docstrings__.copy()
        for name, method in subclass.__dict__.items():
            if callable(method):
                if method.__doc__ is None:
                    method.__doc__ = subclass.__docstrings__.get(name)
                else:
                    subclass.__docstrings__[name] = method.__doc__
