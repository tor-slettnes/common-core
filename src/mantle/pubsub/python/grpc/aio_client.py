'''
AsyncIO Python client for Pub/Sub Relay gRPC service. Publications are added to a send queue and sent asynchronously to the relay in a separate AsyncIO task.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from collections.abc import Iterator, Sequence
import asyncio

### Common Core modules
from cc.core.decorators import override
from cc.core.types import Variant
from cc.core.roundrobin import AsyncQueue
from cc.protobuf.variant import encodeValue, decodeValue

from ..protobuf import Publication, Filters
from .simple_client import SimpleClient, MessageTuple

class AsyncClient (SimpleClient):
    __doc__ = SimpleClient.__doc__

    ### `use_asyncio` tells the `cc.messaging.grpc.Client` base class to create
    ### an AsyncIO gRPC channel.
    use_asyncio = True


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

        SimpleClient.__init__(self,
                              host = host,
                              wait_for_ready = wait_for_ready)
        self._publish_queue = AsyncQueue(queue_size)
        self._publish_task = None

    def __del__(self):
        self._stop_publisher()

    def active(self):
        return self._publish_task and not self._publish_task.done()

    async def publish(self,
                      topic: str,
                      value: Variant,
                      synchronous: bool = False):
        __doc__ = SimpleClient.publish.__doc__

        msg = Publication(
            topic = topic,
            value = encodeValue(value))

        if synchronous:
            await self.stub.Publish(msg)
        else:
            self._start_publisher()
            await self._publish_queue.put(msg)


    def _start_publisher(self):
        if not self.active():
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


    async def subscribe(self,
                        topics: str|Sequence[str]|None = None,
                        wait_for_ready = True,
                        ) -> Iterator[MessageTuple]:
        __doc__ = SimpleClient.subscribe.__doc__

        if isinstance(topics, str):
            topics = (topics,)

        filters = Filters(topics = topics)
        try:
            async for msg in self.stub.Subscriber(filters, wait_for_ready = wait_for_ready):
                yield MessageTuple(msg.topic, decodeValue(msg.value))
        except KeyboardInterrupt:
            print("Cancelling subscription")
