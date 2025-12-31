#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file enumeration.py
## @brief Container for enumerated symbols
## @author Tor Slettnes
#===============================================================================

from collections import UserDict

logging.warning(
    f"Module {__package__}.enumeration is deprecated."
    "Consider using `enum.IntEnum` instead.")


class EnumValue(int):
    '''
    An enumerated value, derived from and compatible with `int`.
    '''

    def __new__ (cls, value, name):
        self = int.__new__(cls, value)
        self.name = name
        return self

    def __repr__ (self):
        return str(self.name)


class Enumeration (dict):
    '''
    A dictionary container mapping integer values to corresponding
    `EnumValue` instances.

    Additionally, these instances are added as attributes corresponding to their
    symbolic name.

    #### Example

    ```python
    >>> from cc.core.enumeration import Enumeration
    >>> my_enum = Enumeration({"ONE":1, "TWO":2})
    >>> my_enum
    {1:ONE, 2:TWO}
    >>> my_enum.ONE
    ONE
    >>> my_enum.ONE == 1
    True
    ```
    '''

    def __init__ (self,
                  enumeration: dict[str,int]|list[tuple[str,int]]|list[str],
                  name = None):
        '''
        Initializer.

        The input is one of the following:
          - A sequence of (name, value) tuples
          - A sequence of names, in which case they will be enumerated from 0.
          - A dictionary-like object with an `items()` method, mapping symbolic
            names to correspoding integer values.
        '''

        self.name = name if name is not None else type(self).__name__

        if hasattr(enumeration, 'items') and callable(enumeration.items):
            items = enumeration.items()

        elif isinstance(enumeration, (list, tuple)):
            sequence = True
            pairs = True
            items = []
            for item in enumeration:
                if (not isinstance(item, tuple)
                    or (len(item) != 2)
                    or not isinstance(item[0], str)
                    or not isinstance(item[1], int)):
                    pairs = False

                if not isinstance(item, str):
                    sequence = False

            if pairs:
                items = enumeration
            elif sequence:
                items = [(name, index) for (index, name) in enumerate(enumeration)]
            else:
                raise TypeError('Enumerated sequence must contain symbols or (symbol, value) tuples')

        else:
            raise TypeError('Argument must be a dictionary, list, or tuple')


        for (name, value) in items:
            self[value] = item = EnumValue(value, name)
            setattr(self, name, item)

    def __repr__ (self):
        return self.name
