'''
Python client for Pub/Sub Relay gRPC service
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from collections.abc import Callable, Iterator, Sequence
from collections import namedtuple
from threading import Thread

### Third party modules
from google.protobuf.message import Message

### Common Core modules
from cc.core.decorators import doc_inherit
from cc.core.types import Variant
from cc.core.roundrobin import Queue
from cc.core.invocation import safe_invoke
from cc.protobuf.variant import encodeValue, decodeValue
from cc.messaging.grpc import Client as BaseClient, ThreadReader, AsyncReader

from ..protobuf import Publication, Filters

MessageTuple = namedtuple('MessageTuple', ('topic', 'value'))


#-------------------------------------------------------------------------------
# Relay Client

class Client (BaseClient):
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

    from .relay_service_pb2_grpc import RelayStub as Stub

    def __init__(
            self,
            host                : str = "",
            wait_for_ready      : bool = False,
            queue_size          : int = 4096,
            use_asyncio         : bool = False,
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

        BaseClient.__init__(self,
                            host = host,
                            wait_for_ready = wait_for_ready,
                            use_asyncio = use_asyncio)

        self._publish_queue = Queue(queue_size)
        self._publish_thread = None
        self._reader = (ThreadReader, AsyncReader)[bool(use_asyncio)]()


    def __del__(self):
        self._stop_publisher()


    def enqueue(self, topic: str,  value: Variant):
        '''
        Enqueue a message for asynchronous publication.
        '''
        self._publish_queue.put(
            Publication(
                topic = topic,
                value = encodeValue(value)))

        self._start_publisher()


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

        return self._publish(publication)


    def _start_publisher(self):
        if not self._publish_thread:
            t = Thread(target = self._publish_worker,
                       daemon = True)

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
                self._publish(publication)

    def _publish(self, publication: Publication):
        return self.stub.Publish(publication)


    def subscribe(self,
                  topics: str|Sequence[str]|None = None,
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
            for msg in self.stub.Subscriber(filters):
                yield MessageTuple(msg.topic, decodeValue(msg.value))
        except KeyboardInterrupt:
            print("Cancelling subscription")
