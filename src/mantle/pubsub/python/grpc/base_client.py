'''
Python base client for Pub/Sub Relay gRPC service.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from typing import abstractmethod, Callable, Iterator, Sequence
from collections import namedtuple
from uuid import uuid1
from dataclasses import dataclass

### Common Core modules
from cc.core.types import Variant
from cc.core.roundrobin import Queue
from cc.core.invocation import safe_invoke
from cc.protobuf.variant import encodeValue, decodeValue
from cc.messaging.grpc import GenericClient

### Pub/Sub module
from ..protobuf import (
    Publication, Filters,
    encodePublication, decodePublication, MessageTuple,
)

SubscriberCallback = Callable[[MessageTuple], None]

@dataclass
class Subscription:
    stream: object
    callback: SubscriberCallback

#-------------------------------------------------------------------------------
# Relay Client

class BaseClient (GenericClient):
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

    write_queue = None

    def __init__(
            self,
            host                : str = "",
            wait_for_ready      : bool = True,
            queue_size          : int = 4096,
    ):
        '''
        Initializer.

        @param host
            Host (resolvable name or IP address) of Relay server

        @param wait_for_ready
            If server is unavailable, keep waiting instead of failing.

        @param queue_size
            Max. number of publications that `enqueue()` will cache locally if
            server is unavailable, before discarding.
        '''

        GenericClient.__init__(self,
                               host = host,
                               wait_for_ready = wait_for_ready)

        self.queue_size = queue_size
        self.subscriptions = {}

    def __del__(self):
        self.stop_writer()


    def publish(self, topic: str,  value: Variant):
        '''
        Publish a message to relevant subscribers via the Pub/Sub Relay.

        In case the relay is unreachable, this call may fail or block, depending
        on the value of the `self.wait_for_ready` as given to the constructor.
        To enqueue a message without blocking, see `enqueue()`.

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
        return self.stub.Publish(encodePublication(topic, value))


    def enqueue(self,
                topic: str,
                value: Variant):
        '''
        Enqueue a message for asynchronous publication via the Pub/Sub relay.

        This call returns immediately, even if the Pub/Sub Relay is unreachable.
        If the queue is full, the oldest item already in the queue is discarded.
        If you need guaranteed, invoke (and await) `publish()` command instead.

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

        self.start_writer()
        return self.write_queue.put_roundrobin(
            encodePublication(topic, value))


    @abstractmethod
    def writer_active(self) -> bool:
        '''Indicate whether message writer is currently open'''
        pass

    @abstractmethod
    def start_writer(self):
        '''Start message writer if not running'''

    @abstractmethod
    def stop_writer(self):
        '''Stop message writer if running'''

    @abstractmethod
    def listen(self,
               topics: str|Sequence[str]|None = None,
               wait_for_ready = True,
               ) -> Iterator[Publication]:
        '''
        Subscribe and listen to message publications from the Pub/Sub Relay
        service.

        @param topics
            Receive publications only on the specified topics.
            If omitted, publications on any topic will be received.

        @return
            An iterator over messages published from the relay

        ### Example usage:

        ```
        $ python3
        >>> from cc.platform.pubsub.grpc import Client as RelayClient
        >>> client = RelayClient(RELAY_HOST)
        >>> for topic, value_ in client.listen(('my_first_topic', 'my_second_topic')):
        ...    print(f"{topic=}, {value=}")
        ```
        '''

        if isinstance(topics, str):
            topics = (topics,)

        filters = Filters(topics = topics)
        return self.stub.Subscriber(filters, wait_for_ready = wait_for_ready)


    def subscribe(self, *,
                  callback: SubscriberCallback,
                  topics: str|Sequence[str]|None = None,
                  handle: str|None = None) -> str:
        '''
        Subscribe asynchronously to message publications from the Pub/Sub Relay.

        @param handler
            Callback handler to invoke when a matching publication is received
            from the Relay.

        @param topics
            Receive publications only on the specified topics.  If omitted,
            publications on any topic will be received.

        @param handle
            Unique identifier for this subscription. If omitted, one is generated
            using `uuid.uuid1()`.

        @return
            Unique identifier that was provided or assigned.  This may
            subsequently be used to cancel the subscription via `unsubscribe()`.
        '''

        if not handle:
            handle = str(uuid1())

        if isinstance(topics, str):
            topics = (topics,)

        filters = Filters(topics = topics)
        reader  = self.stub.Subscriber(filters, wait_for_ready=True)
        subscription = Subscription(stream = reader, callback = callback)
        self.subscriptions[handle] = subscription
        self.start_reader(subscription)
        return handle


    def unsubscribe(self, handle: str) -> bool:
        '''
        Cancel an existing message subscription.

        @param handle
            Subscription handle returned from the original `subscribe()` request.

        @return
            True if the subscription was found and canceled

        @note
            If no more subscriptions are active, the worker that reads message
            publications is closed.
        '''

        if subscription := self.subscriptions.pop(handle, None):
            self.stop_reader(subscription)
            return True
        else:
            return False


    def unsubscribe_all(self) -> bool:
        '''
        Cancel all message subscriptions held by this client.

        @return
            True if any subscriptions were found and canceled
        '''

        found = False
        for subscription in self.subscriptions.values():
            self.stop_reader(subscription)
            found = True

        self.subscriptions.clear()
        return found


    @abstractmethod
    def start_reader(self, subscription: Subscription):
        '''Start message reader'''


    @abstractmethod
    def stop_reader(self, subscription: Subscription):
        '''Stop message reader'''

