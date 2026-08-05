'''
ProtoBuf type wrappers for Pub/Sub Relay service
'''

__author__ = "Tor Slettnes"

## Standard Python modules
from collections import namedtuple
from typing import Sequence, Mapping

## Common Core modules
from cc.core.types import Variant

## Pub/Sub modules
from .pubsub_types_pb2 import (
    Publication, Topics,
    ReplayPolicy, ReplayPolicyMap, KeyPath,
)

from cc.protobuf.variant import Value, encodeValue, decodeValue

MessageTuple         = namedtuple('MessageTuple', ('topic', 'value'))
TopicName            = str
TopicsInput          = Topics|Sequence[TopicName]
MappingKeys          = Sequence[str]
ReplayPolicyInput    = ReplayPolicy|MappingKeys
ReplayPolicyMapInput = ReplayPolicyMap|Mapping[TopicName,ReplayPolicyInput]


def encodeTopics(topics: TopicsInput) -> Topics:
    if isinstance(topics, Topics):
        return topics

    elif isinstance(topics, str):
        return Topics(
            topics = [topics],
        )

    elif isinstance(topics, Sequence):
        return Topics(
            topics = topics,
        )

    else:
        raise TypeError(
            "Invalid `topics` input; expected `%s' or a sequence, got %s: %s"%(
                Topics.__qualname__,
                type(topics).__qualname__,
                topics,
            ))


def encodePublication(topic: str, value: Variant) -> Publication:
    return Publication(topic = topic,
                       value = encodeValue(value))

def decodePublication(msg: Publication) -> MessageTuple:
    return MessageTuple(topic = msg.topic,
                        value = decodeValue(msg.value))


def encodeKeyPath(key_path: KeyPath|Sequence[str]|str,
                  path_delimiter: str = "/") -> KeyPath:

    if isinstance(key_path, KeyPath):
        return key_path

    elif isinstance(key_path, str):
        return KeyPath(elements = key_path.split(path_delimiter))

    elif isinstance(key_path, Sequence):
        return KeyPath(elements = key_path)

    else:
        raise TypeError(
            "Invalid `key_path` input; expected `%s' or a sequence, got %s: %s"%(
                KeyPath.__qualname__,
                type(key_path).__qualname__,
                key_path,
            ))


def encodeReplayPolicy(policy: ReplayPolicyInput|None) -> ReplayPolicy:
    if isinstance(policy, ReplayPolicy):
        return policy

    elif isinstance(policy, str):
        return ReplayPolicy(
            replay_latest = True,
            key_paths = [encodeKeyPath(policy)],
        )

    elif isinstance(policy, Sequence):
        return ReplayPolicy(
            replay_latest = True,
            key_paths = [encodeKeyPath(path) for path in policy],
        )

    elif policy is None:
        return ReplayPolicy(
            replay_latest = True,
        )

    else:
        raise TypeError(
            "Invalid `policy` input; expected `%s' or a sequence, got %s: %s"%(
                ReplayPolicy.__qualname__,
                type(policy).__qualname__,
                policy,
            ))

def encodeReplayPolicyMap(policy_map: ReplayPolicyMapInput) -> ReplayPolicyMap:
    if isinstance(policy_map, ReplayPolicyMap):
        return policy_map

    elif isinstance(policy_map, Mapping):
        return ReplayPolicyMap(
            map = {
                topic: encodeReplayPolicy(policy)
                for (topic, policy) in policy_map.items()
            })

    else:
        raise TypeError(
            "Invalid `policy_map` input; expected `%s' or a mapping, got %s: %s"%(
                ReplayPolicyMap.__qualname__,
                type(policy_map).__qualname__,
                policy_map,
            ))
