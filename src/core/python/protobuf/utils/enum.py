'''
enum.py - Utility functions for enumerated ProtoBuf types
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard Python modules
from enum import IntEnum

### Third-party modules
from google.protobuf.internal.enum_type_wrapper import EnumTypeWrapper

### Common Core modules
from cc.core.stringutils import common_prefix

#===============================================================================
# Methods

def enum_from_proto(proto_type: EnumTypeWrapper,
                    strip_common_prefix: bool = True) -> IntEnum:
    '''
    Convert a ProtoBuf `enum` type to Python `enum.IntEnum`.  These are
    interchangable as both are derived from `int`, but the latter provides for
    better documentation/introspective representation.

    If `strip_common_prefix` is set, any leading underscore-separated subwords
    that are common to all symbols in the ProtoBuf enumeration are stripped off
    in the resulting Python enumeration. This is the default behavior.

    For example, consider the ProtoBuf definition:

    ```proto
    enum MyEnum
    {
        MY_ENUM_UNSPECIFIED = 0;
        MY_ENUM_FIRST       = 1;
        MY_ENUM_SECOND      = 2;
    }
    ```

    With the `strip_common_prefix` option set, this would result in a Python
    enumeration equivalent to:

    ```python
    class MyEnum (IntEnum):
        UNSPECIFIED = 0
        FIRST       = 1
        SECOND      = 2
    ```
    '''

    if strip_common_prefix:
        prefix_length, prefix = common_prefix(proto_type.keys())
        symbols = [key[prefix_length:] for key in proto_type.keys()]
        items = zip(symbols, proto_type.values())
    else:
        items = proto_type.items()

    return IntEnum(proto_type.DESCRIPTOR.name, items)
