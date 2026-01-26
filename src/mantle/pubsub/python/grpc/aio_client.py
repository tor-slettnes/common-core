'''
AsyncIO Python client for Pub/Sub Relay gRPC service.

Example usage:

* Create a Pub/Sub AsyncClient instance:

  ```
  python3 -m asyncio
  >>> from cc.platform.pubsub.grpc.aio_client import AsyncClient, MessageTuple
  >>> pubsub = AsyncClient("localhost")
  ```

* Listen for specific topics in the foreground:

  ```
  >>> async for msg in pubsub.listen(topics=("my topic",)):
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

* Add a asynchronoous subscription:

  ```
  >>> def my_subscriber(msg: MessageTuple):
  ...     """callback handler, invoked asynchronously on matching message publications"""
  ...     print(f"{msg=}\r")
  ...
  >>> handle = pubsub.subscribe(topics=["my topic"], callback=my_subscriber)
  ```

  **Note:** The callback handler may itself optionally be an `async` function, in which
  case it is awaited in the worker task.


* Publish a message via the Relay.  The subscriber above will be invoked from an asynchronous worker task:

  ```
  >>> await pubsub.publish("my topic", {"one":True, "two":2, "three":3.141592653589793238, "four":"IV"})

  msg=MessageTuple(topic='my topic', value={'three': 3.141592653589793, 'two': 2, 'one': True, 'four': 'IV'})
  >>>
  ```

* Enqueue a message for asynchronous publication.

  ```
  >>> pubsub.enqueue("my topic", {"five":False, "six":[1,2,3]})
  >>> msg=MessageTuple(topic='my topic', value={'six': [1, 2, 3], 'five': False})
  ```

  **Note:** Upon first invocation, `enqueue()` creates a publication queue, as well as a and a worker task to read from this queue and publish the messages in the background.  The preferred queue size can be provided int the `Client()` constructor, above.


* Cancel the subscription

  ```
  >>> pubsub.unsubscribe(handle)
  ```

'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from collections.abc import Iterator, Sequence
import asyncio

### Common Core modules
from cc.core.decorators import override, doc_inherit
from cc.core.types import Variant
from cc.core.roundrobin import AsyncQueue
from cc.protobuf.variant import encodeValue, decodeValue
from cc.messaging.grpc.client import AsyncMixIn

from ..protobuf import  Filters
from .client import Client, MessageTuple, SubscriberCallback, Subscription

class AsyncClient (AsyncMixIn, Client):
    __doc__ = Client.__doc__

    _writer_task = None
    _reader_task = None

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

        Client.__init__(self,
                        host = host,
                        wait_for_ready = wait_for_ready)

        self.reader_tasks = set()

    def __del__(self):
        self._stop_writer()

    @doc_inherit
    async def publish(self, topic: str, value: Variant):
        ### This simply overrides the parent to provide a more descriptive
        ### method signature.
        await Client.publish(self, topic, value)


    @override
    async def listen(self,
                     topics: str|Sequence[str]|None = None,
                     wait_for_ready = True,
                     ) -> Iterator[MessageTuple]:
        '''
        Subscribe and listen to message publications from the Pub/Sub Relay.

        @param topics
            Receive publications only on the specified topics.
            If omitted, publications on any topic will be recieved.

        @return
            An iterator over messages published from the relay

        ### Example usage:

        ```
        $ python3 -m asyncio
        >>> from cc.platform.pubsub.grpc import AsyncClient
        >>> client = AsyncClient(RELAY_HOST)
        >>> async for topic, value_ in client.listen(('my_first_topic', 'my_second_topic')):
        ...    print(f"{topic=}, {value=}")
        ```
        '''

        if isinstance(topics, str):
            topics = (topics,)

        filters = Filters(topics = topics)
        try:
            async for msg in self.stub.Subscriber(filters, wait_for_ready = wait_for_ready):
                yield self.decode_publication(msg)
        except KeyboardInterrupt:
            print("Cancelling subscription")

    def writer_active(self):
        return self._writer_task and not self._writer_task.done()

    def _start_writer(self):
        if not self.writer_active():
            self._write_queue = AsyncQueue(self.queue_size)
            self._writer_task = asyncio.create_task(
                self._write_worker(),
                name = "Message Writer")

    def _stop_writer(self):
        if task := self._writer_task:
            self._writer_task = None
            task.cancel()

    async def _write_worker(self):
        while self._writer_task:
            while publication := await self._write_queue.get():
                await self.stub.Publish(publication)

    def _start_reader(self, subscription: Subscription):
        task = asyncio.create_task(
            self._read_worker(subscription),
            name = "Message Reader")
        self.reader_tasks.add(task)
        task.add_done_callback(self.reader_tasks.discard)

    def _stop_reader(self, subscription: Subscription):
        subscription.callback = None
        if stream := subscription.stream:
            stream.cancel()

    async def _read_worker(self, subscription: Subscription):
        async for msg in subscription.stream:
            if callback := subscription.callback:
                result = callback(self.decode_publication(msg))
                if asyncio.iscoroutine(result):
                    await result
