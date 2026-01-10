'''
AsyncIO Python client for Pub/Sub Relay gRPC service.
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

from ..protobuf import Publication, Filters
from .client import Client, MessageTuple

class AsyncClient (AsyncMixIn, Client):
    __doc__ = Client.__doc__

    _publish_task = None

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

    def __del__(self):
        self._stop_publisher()

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
        Subscribe and listen to message publications from the Pub/Sub Relay
        service.

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
                yield MessageTuple(msg.topic, decodeValue(msg.value))
        except KeyboardInterrupt:
            print("Cancelling subscription")


    def active(self):
        return self._publish_task and not self._publish_task.done()

    def _start_publisher(self):
        if not self.active():
            self._publish_queue = AsyncQueue(self.queue_size)
            self._publish_task = asyncio.create_task(
                self._publish_worker())

    def _stop_publisher(self):
        if task := self._publish_task:
            self._publish_task = None
            task.cancel()

    async def _publish_worker(self):
        while self._publish_task:
            while publication := await self._publish_queue.get():
                await self.stub.Publish(publication)

