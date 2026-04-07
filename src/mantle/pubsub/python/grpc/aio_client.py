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

  **Note:** Upon first invocation, `enqueue()` creates a publication queue, as well as a and a worker task to read from this queue and publish the messages in the background.  The preferred queue size can be provided in the `Client()` constructor.


* Cancel the subscription

  ```
  >>> pubsub.unsubscribe(handle)
  ```

'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from typing import Iterator, Sequence
import asyncio

### Common Core modules
from cc.core.decorators import override
from cc.core.roundrobin import AsyncQueue
from cc.core.types import Variant
from cc.messaging.grpc.client import AsyncMixIn

### Pub/Sub modules
from ..protobuf import  MessageTuple, decodePublication
from .base_client import BaseClient,  Subscription


class AsyncClient (AsyncMixIn, BaseClient):
    __doc__ = BaseClient.__doc__

    writer_task = None

    def __init__(self,
                 host                : str = "",
                 wait_for_ready      : bool = True,
                 queue_size          : int = 4096,
                 ):

        BaseClient.__init__(**locals())
        self.reader_tasks = set()

    @override
    async def publish(self, topic: str, value: Variant):
        await BaseClient.publish(self, topic, value)

    @override
    def writer_active(self):
        return self.writer_task and not self.writer_task.done()

    @override
    async def listen(self,
                     topics: str|Sequence[str]|None = None,
                     wait_for_ready = True,
                     ) -> Iterator[MessageTuple]:
        try:
            async for msg in BaseClient.listen(**locals()):
                yield decodePublication(msg)
        except KeyboardInterrupt:
            print("Cancelling subscription")

    @override
    def start_writer(self):
        if not self.writer_active():
            self.write_queue = AsyncQueue(self.queue_size)
            self.writer_task = asyncio.create_task(
                self._write_worker(),
                name = "Message Writer")

    @override
    def stop_writer(self):
        if task := self.writer_task:
            self.writer_task = None
            task.cancel()

    async def _write_worker(self):
        while self.writer_task:
            while publication := await self.write_queue.get():
                await self.stub.Publish(publication)

    @override
    def start_reader(self, subscription: Subscription):
        task = asyncio.create_task(
            self._read_worker(subscription),
            name = "Message Reader")
        self.reader_tasks.add(task)
        task.add_done_callback(self.reader_tasks.discard)

    @override
    def stop_reader(self, subscription: Subscription):
        subscription.callback = None
        if stream := subscription.stream:
            stream.cancel()

    async def _read_worker(self, subscription: Subscription):
        async for msg in subscription.stream:
            if callback := subscription.callback:
                msg = decodePublication(msg)
                try:
                    result = callback(msg)
                    if asyncio.iscoroutine(result):
                        await result
                except Exception as e:
                    self.logger.error(
                        'Subscription handler %r(%s) failed: [%s] %s' % (
                            callback.__name__,
                            msg,
                            type(e).__name__,
                            e,
                        ))
