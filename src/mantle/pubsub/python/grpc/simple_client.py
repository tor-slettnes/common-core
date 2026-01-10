'''
Simple Python client for Pub/Sub Relay gRPC service, without a send queue.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from collections.abc import Iterator, Sequence
from collections import namedtuple

### Common Core modules
from cc.core.types import Variant
from cc.protobuf.variant import encodeValue, decodeValue
from cc.messaging.grpc import GenericClient

### Pub/Sub module
from ..protobuf import Publication, Filters

MessageTuple = namedtuple('MessageTuple', ('topic', 'value'))

#-------------------------------------------------------------------------------
# Relay Client

class SimpleClient (GenericClient):
    '''
    Python gRPC client for the Pub/Sub Relay.  This Relay is an intermediary
    message broker between producers and consumers of asynchronous events and
    data. Producers publish data through the relay to any subscribing consumers.

    The Relay supports multiple payload formats and transports. A producer may
    send a ProtoBuf message over gRPC, whereas a consumer may then receive that
    message as JSON text over ZeroMQ - or vice versa.

    A message publication comprises two fields: A topic and a value.

    * The topic is a string. A consumer may filter publications by subscribing
      only to specific topics.

    * The value is a Variant. Acceptable types include:
      - A simple built-in type: None, bool, int, float, str, bytes
      - A relative time interval (see `cc.core.timeutils.TimeInterval`)
      - An absolute time point (see `cc.core.timeutils.TimePoint`)
      - A sequence (e.g. list) of nested Variant values
      - A mapping (e.g. dictionary) of string keys to nested Variant values
      - An ordered (tag, value) sequence, where tags need not be unique.

      Some of these types may not be perfectly reconstructable once the Relay
      converts them to JSON text for publication to ZMQ subscribers. For
      example:
      - byte arrays are represented as base64-encoded strings;
      - relative time intervals and absolute time points are both represented as
        ISO8601 compliant strings (see https://en.wikipedia.org/wiki/ISO_8601);
      - a tag/value list will be represented as an array of nested arrays,
        each comprising two values: a string and a Variant value.
    '''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `cc.messaging.grpc.GenericClient` base to instantiate `self.stub`.
    from .relay_service_pb2_grpc import RelayStub as Stub

    def publish(self, topic: str,  value: Variant):
        '''
        Publish a message to relevant subscribers via the Pub/Sub Relay.

        @param topic
            Message topic, used by subscribers to filter and/or process messages
            accordingly.

        @param value
            Message data. Acceptable value types include:
            - A simple built-in type: None, bool, int, float, str, bytes
            - A relative time interval (see `cc.core.timeutils.TimeInterval`)
            - An absolute time point (see `cc.core.timeutils.TimePoint`)
            - A sequence (e.g. list) of nested Variant values
            - A mapping (e.g. dictionary) of string keys to nested Variant values
            - An ordered (tag, value) sequence, where tags need not be unique.
        '''
        publication = Publication(
            topic = topic,
            value = encodeValue(value))

        return self.stub.Publish(publication)

    def subscribe(self,
                  topics: str|Sequence[str]|None = None,
                  wait_for_ready = True,
                  ) -> Iterator[MessageTuple]:
        '''
        Subscribe to message publications from the Pub/Sub Relay service.

        @param topics
            Receive publications only on the specified topics.
            If omitted, publications on any topic will be recieved.

        @return
            An iterator over messages published from the relay
        '''

        if isinstance(topics, str):
            topics = (topics,)

        filters = Filters(topics = topics)
        try:
            for msg in self.stub.Subscriber(filters, wait_for_ready = wait_for_ready):
                yield MessageTuple(msg.topic, decodeValue(msg.value))
        except KeyboardInterrupt:
            print("Cancelling subscription")
