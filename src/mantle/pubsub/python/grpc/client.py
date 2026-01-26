'''
Python client for Pub/Sub Relay gRPC service.

Example usage:

* Create a Pub/Sub AsyncClient instance:

  ```
  python3
  >>> from cc.platform.pubsub.grpc.client import Client, MessageTuple
  >>> pubsub = Client("localhost")
  ```

* Listen for specific topics in the foreground:

  ```
  >>> for msg in pubsub.listen(topics=("my topic",)):
  ...     print(f"{msg=}\r")
  ```

  (This blocks until cancelled, e.g. via **[Ctrl]** + **[C]**)

* Publish a message from a new terminal, e.g. using `cc-pubsub-tool`

  ```
  cc-pubsub-tool publish 'my topic' '{"one": true, "two": 2, "three": 3.141592653589793238, "four":"IV"}' --json
  ```

* Observe that the message appears in the first terminal

  ```
  msg=MessageTuple(topic='my topic', value={'four': 'IV', 'two': 2, 'three': 3.141592653589793, 'one': True})
  ```

* Add a subscription:

  ```
  >>> def my_subscriber(msg: MessageTuple):
  ...     """callback handler, invoked asynchronously on matching message publications"""
  ...     print(f"{msg=}\r")
  ...
  >>> handle = pubsub.subscribe(topics=["my topic"], callback=my_subscriber)
  ```

* Publish a message via the Relay.  The subscriber above will be invoked from an asynchronous worker thread:

  ```
  >>> pubsub.publish("my topic", {"one":True, "two":2, "three":3.141592653589793238, "four":"IV"})

  msg=MessageTuple(topic='my topic', value={'three': 3.141592653589793, 'two': 2, 'one': True, 'four': 'IV'})
  >>>
  ```

* Enqueue a message for asynchronous publication.

  ```
  >>> pubsub.enqueue("my topic", {"five":False, "six":[1,2,3]})
  >>> msg=MessageTuple(topic='my topic', value={'six': [1, 2, 3], 'five': False})
  ```

  **Note:** Upon first invocation, `enqueue()` creates a publication queue, as well as a and a worker thread to read from this queue and publish the messages in the background.  The preferred queue size can be provided int the `Client()` constructor, above.


* Cancel the subscription

  ```
  >>> pubsub.unsubscribe(handle)
  ```

'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from collections.abc import Callable, Iterator, Sequence
from collections import namedtuple
from threading import Thread
from uuid import uuid1
from dataclasses import dataclass

### Third party modules
import grpc

### Common Core modules
from cc.core.types import Variant
from cc.core.roundrobin import Queue
from cc.core.invocation import safe_invoke
from cc.protobuf.variant import encodeValue, decodeValue
from cc.messaging.grpc import GenericClient

### Pub/Sub module
from ..protobuf import Publication, Filters

MessageTuple = namedtuple('MessageTuple', ('topic', 'value'))
SubscriberCallback = Callable[[MessageTuple], None]

@dataclass
class Subscription:
    stream: object
    callback: SubscriberCallback

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

    _writer_thread = None

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
        self._stop_writer()


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
        return self.stub.Publish(self.encode_publication(topic, value))


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

        self._start_writer()
        return self._write_queue.put_roundrobin(
            self.encode_publication(topic, value))


    def writer_active(self):
        if t := self._writer_thread:
            return t.is_alive()
        else:
            return False

    def encode_publication(self,
                           topic: str,
                           value: Variant) -> Publication:
        return Publication(topic = topic, value = encodeValue(value))

    def decode_publication(self, msg):
        return MessageTuple(topic = msg.topic, value = decodeValue(msg.value))


    def _start_writer(self):
        if not self.writer_active():
            t = Thread(target = self._write_worker,
                       daemon = True)
            self._write_queue = Queue(self.queue_size)
            self._writer_thread = t
            t.start()

    def _stop_writer(self):
        if t := self._writer_thread:
            self._writer_thread = None
            self._write_queue.put(None)
            t.join()

    def _write_worker(self):
        while self._writer_thread:
            while publication := self._write_queue.get():
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
                yield self.decode_publication(msg)
        except KeyboardInterrupt:
            print("Cancelling subscription")


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
        self._start_reader(subscription)
        return handle


    def unsubscribe(self, handle: str) -> bool:
        '''
        Cancel an existing message subscription.

        @param handle
            Subscription handle returned from the original `subscribe()` request.

        @return
            True if the subscription was found and canceled

        @note
            If no more subscriptions are active, the worker thread that reads
            message publications is closed.
        '''

        if subscription := self.subscriptions.pop(handle, None):
            self._stop_reader(subscription)
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
            self._stop_reader(subscription)
            found = True

        self.subscriptions.clear()
        return found


    def _start_reader(self, subscription: Subscription):
        thread  = Thread(target = self._read_worker, args=(subscription,), daemon = True)
        thread.start()

    def _stop_reader(self, subscription: Subscription):
        try:
            subscription.stream.cancel()
        except AttributeError:
            ## Readers from older `grpcio` verisons do not have a `cancel()` method
            subscription.stream.throw(StopIteration)

        ### Older versions of the standard Python gRPC client stream do not have
        ### a `cancel()` method.  There is a `stop()`, but attempt to invoke
        ### this on an active stream raises a ValueError.  So in lieu of
        ### cancelling the stream, we simply set the `callback` field to
        ### None. The worker thread will then exit after reading the next
        ### publication from the relay.
        subscription.callback = None

    def _read_worker(self, subscription: Subscription):
        for msg in subscription.stream:
            if callback := subscription.callback:
                ### We are still subscribed.
                safe_invoke(
                    function = callback,
                    args = (self.decode_publication(msg),),
                )
            else:
                ### We the client unubscribed while we were waiting for the next message.
                ### Exit the thread here.
                break


if __name__ == "__main__":
    client = Client()
    handle = client.subscribe(callback = lambda publication: print(f"{publication=}\r"))
