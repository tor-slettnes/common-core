'''
ProtoBuf type wrappers for Pub/Sub Relay service
'''

__author__ = "Tor Slettnes"

## Standard Python modules
from collections import namedtuple

## Common Core modules
from cc.core.types import Variant

## Pub/Sub modules
from .pubsub_types_pb2 import Publication
from cc.protobuf.variant import Value, encodeValue, decodeValue

MessageTuple = namedtuple('MessageTuple', ('topic', 'value'))

def encodePublication(topic: str, value: Variant) -> Publication:
    return Publication(topic = topic,
                       value = encodeValue(value))

def decodePublication(msg: Publication) -> MessageTuple:
    return MessageTuple(topic = msg.topic,
                        value = decodeValue(msg.value))
