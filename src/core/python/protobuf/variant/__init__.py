'''
Package `cc.protobuf.variant` - containers for generic data types:

 - any of the following basic types: boolean, signed/unsigned integer, real number, string, byte sequence
 - any of the following well-known types: Timestamp, Duration
 - any of the nested value containers below: ValueList, TaggedValueList, ValueMap

'''

from .variant import \
    PyValue, Value, encodeValue, decodeValue, \
    PyValueList, ValueList, encodeValueList, decodeValueList, \
    PyTaggedValue, TaggedValue, encodeTaggedValue, decodeTaggedValue, \
    PyTaggedValueList, TaggedValueList, encodeTaggedValueList, decodeTaggedValueList, \
    PyValueDict, KeyValueMap, encodeKeyValueMap, decodeKeyValueMap, keyValueMap, \
    is_tagged_list
