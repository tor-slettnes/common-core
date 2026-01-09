'''
Threaded Python client for Pub/Sub Relay gRPC service. Publications are added to a send queue and sent asynchronously to the relay in a separate thread.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from collections.abc import Iterator
from threading import Thread

### Common Core modules
from cc.core.decorators import override
from cc.core.types import Variant
from cc.core.roundrobin import Queue
from cc.protobuf.variant import encodeValue

from ..protobuf import Publication
from .simple_client import SimpleClient

class Client (SimpleClient):
    __doc__ = SimpleClient.__doc__

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
        self._publish_thread = None
        self._publish_queue = Queue(queue_size)

    def __del__(self):
        self._stop_publisher()

    def active (self):
        if t := self._publish_thread:
            return t.is_alive()
        else:
            return False

    def publish(self,
                topic: str,
                value: Variant,
                synchronous: bool = False):
        __doc__ = SimpleClient.publish.__doc__

        msg = Publication(
            topic = topic,
            value = encodeValue(value))

        if synchronous:
            return self.stub.Publish(msg)
        else:
            self._start_publisher()
            return self._publish_queue.put(msg)


    def _start_publisher(self):
        if not self.active():
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
                self.stub.Publish(publication, wait_for_ready=True)
