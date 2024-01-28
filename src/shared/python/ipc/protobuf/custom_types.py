#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file protobuf_custom_types.py
## @brief Convience wrappers for Common Core shared types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from .standard_types import ProtoBuf

class CC (object):
    import event_types_pb2 as Status
    import signal_types_pb2 as Signal
    import variant_types_pb2 as Variant

    @classmethod
    def encodeValue(cls, value : object):
        tv = CC.Variant.Value()

        if isinstance(value, tuple):
            tag, value = value
            tv.tag = tag or ""

        if isinstance(value, bool):
            tv.value_bool = value

        elif isinstance(value, int) and (value >= 0):
            tv.value_uint = value

        elif isinstance(value, int):
            tv.value_sint = value

        elif isinstance(value, float):
            tv.value_real = value

        elif isinstance(value, str):
            tv.value_string = value

        elif isinstance(value, bytes):
            tv.value_bytes = value

        elif isinstance(value, ProtoBuf.Timestamp):
            tv.value_timestamp.CopyFrom(value)

        elif isinstance(value, ProtoBuf.Duration):
            tv.value_duration.CopyFrom(value)

        elif isinstance(value, list):
            tv.value_list.items.extend([cls.encodeValue(item) for item in value])

        elif isinstance(value, dict):
            tv.value_list.items.extend([cls.encodeValue(item) for item in value.items()])
            tv.value_list.mappable = True

        elif value is not None:
            raise TypeError("Cannot convert type %s to protobuf value: %s"%
                            (type(value), value))
        return tv


    @classmethod
    def decodeValue(cls, v, autotype=True):
        fieldname = v.WhichOneof('value')

        if (fieldname == 'value_timestamp'):
            value = cls.decodeTimestamp(v.value_timestamp)
        elif (fieldname == 'value_duration'):
            value = cls.decodeDuration(v.value_duration)
        elif (fieldname == 'value_list'):
            if autotype and v.value_list.untagged:
                value = [ cls.decodeValue(tv, autotype)
                          for tv in v.value_list.items ]
            else:
                value = [ cls.decodeTaggedValue(tv, autotype)
                          for tv in v.value_list.items ]
                if autotype and v.value_list.mappable:
                    value = dict(value)
        elif fieldname:
            value = getattr(v, fieldname)
        else:
            value = None
        return value


    @classmethod
    def decodeTaggedValue(cls, tv, autotype=True):
        tag = None
        if tv and tv.tag:
            tag = tv.tag
            value = cls.decodeValue(tv, autotype)
        return (tag, value)

    @classmethod
    def decodeTimestamp(cls, prototime):
        return prototime.seconds + prototime.nanos*1e-9

    @classmethod
    def encodeTimestamp(cls, timestamp):
        if timestamp:
            return ProtoBuf.Timestamp(seconds=int(timestamp),
                                nanos=int((timestamp-int(timestamp))*1e9))

    @classmethod
    def decodeDuration(cls, duration):
        return duration.seconds + duration.nanos*1e-9


    @classmethod
    def encodeDuration(cls, duration):
        if duration:
            return ProtoBuf.Duration(seconds=int(duration),
                               nanos=int((duration-int(duration))*1e9))


    @classmethod
    def encodeValueList(cls, value):
        if value is None:
            return CC.Variant.ValueList()

        elif isinstance(value, dict):
            return CC.Variant.ValueList(
                items = [cls.encodeValue(tv) for tv in value.items()],
                mappable = True)

        elif isinstance(value, list):
            return CC.Variant.ValueList(
                items = [cls.encodeValue(v) for v in value])

        elif isinstance(value, CC.Variant.ValueList):
            return value

        else:
            raise ValueError("encodeValueList() expects a dictionary or list")


    @classmethod
    def decodeTaggedValueList(cls, valuelist, autotype=True):
        if isinstance(valuelist, CC.Variant.ValueList):
            return [cls.decodeTaggedValue(tv, autotype) for tv in valuelist.items]
        else:
            raise ValueError("Argument must be a CC.Variant.ValueList() instance")

    @classmethod
    def decodeValueList(cls, valuelist, autotype=True):
        return [v for (t,v) in self.decodeTaggedValueList(valuelist, autotype)]

    @classmethod
    def decodeValueMap(cls, valuelist, autotype=True):
        return dict(cls.decodeTaggedValueList(valuelist, autotype))

