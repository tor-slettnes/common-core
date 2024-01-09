#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file container.py
## @brief Interactive wrapper for gRPC-based Instrument Services
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

class Container (object):
    '''General purpose data container. Values may be provided on instantiation, e.g.,
    ```
    >>> my_container = Container(one=True, two=2, three=3.141592653589793238, four="IV")
    ```

    Values may subseqently be assigned or accessed either as instance attributes,
    or as elements in a dictionary:

    ```
    >>> my_container.four
    'IV'
    >>> my_container['four']
    'IV'
    >>> my_container.five = 5.0
    >>> my_container['five']
    5.0
    ```
    '''

    def __init__ (self, _keys=(), **kwargs):
        object.__setattr__(self, '_data', kwargs)
        object.__setattr__(self, '_keys', _keys)

    def __repr__ (self):
        items = []
        keys = set(self._data.keys())
        for key in self._keys:
            try:
                value = self._data[key]
            except KeyError:
                pass
            else:
                if value:
                    items.append((key, value))

            keys.discard(key)

        for key in keys:
            value = self._data.get(key)
            if value:
                items.append((key, value))

        return "%s(%s)"%(type(self).__name__,
                         ", ".join(["%s=%r"%item for item in items]))

    def __getattr__ (self, key):
        try:
            return self._data[key]
        except KeyError:
            raise AttributeError(key) from None

    def __setattr__ (self, key, value):
        self._data[key] = value

    def __getitem__ (self, key):
        return self._data[key]

    def __setitem__ (self, key, value):
        self._data[key] = value

