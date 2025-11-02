'''
Package `cc.protobuf.datetime` - support for Protobuf types from `datetime.proto`
'''
from .datetime import TimeStruct, Weekday, Interval, TimeUnit, \
    encodeInterval, encodeTimeStruct, decodeTimeStruct
