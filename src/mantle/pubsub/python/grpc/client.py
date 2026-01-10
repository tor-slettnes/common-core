'''
Python client for Pub/Sub Relay gRPC service.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from collections.abc import Iterator, Sequence
from collections import namedtuple
from threading import Thread

### Common Core modules
from cc.core.types import Variant
from cc.core.roundrobin import Queue
from cc.protobuf.variant import encodeValue, decodeValue
from cc.messaging.grpc import GenericClient

### Pub/Sub module
from ..protobuf import Publication, Filters

MessageTuple = namedtuple('MessageTuple', ('topic', 'value'))

#-------------------------------------------------------------------------------
# Relay Client

class Client (GenericClient):
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

    _publish_thread = None

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

    def __del__(self):
        self._stop_publisher()

    def active (self):
        if t := self._publish_thread:
            return t.is_alive()
        else:
            return False


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
        publication = Publication(
            topic = topic,
            value = encodeValue(value))

        return self.stub.Publish(publication)


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

        msg = Publication(
            topic = topic,
            value = encodeValue(value))

        self._start_publisher()
        return self._publish_queue.put_roundrobin(msg)

    def _start_publisher(self):
        if not self.active():
            t = Thread(target = self._publish_worker,
                       daemon = True)
            self._publish_queue = Queue(self.queue_size)
            self._publish_thread = t
            t.start()

    def _stop_publisher(self):
        if t := self._publish_thread:
            self._publish_thread = None
            self._publish_queue.put(None)
            t.join()

    def _publish_worker(self):
        while self._publish_thread:
            while publication := self._publish_queue.get():
                self.stub.Publish(publication, wait_for_ready=True)

    def listen(self,
               topics: str|Sequence[str]|None = None,
               wait_for_ready = True,
               ) -> Iterator[MessageTuple]:
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
        try:
            for msg in self.stub.Subscriber(filters, wait_for_ready = wait_for_ready):
                yield MessageTuple(msg.topic, decodeValue(msg.value))
        except KeyboardInterrupt:
            print("Cancelling subscription")
