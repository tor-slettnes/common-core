'''
Specialized queue with configurable overflow disposition
'''

__author__ = "Tor Slettnes"
__docformat__ = "javadoc en"

import queue
import asyncio
import threading
import multiprocessing.queues

class RoundRobin:
    '''
    Mix-in class to add overflow handling to Queue-like objects
    (`queue.Queue()`, `asyncio.Queue()`, `multiprocessing.Queue()`).

    See the `enqueue()` method for more details.
    '''


    def put_roundrobin (self, item: object):
        '''
        Put an item in the queue.

        This behaves similar to `put()` / `put_nowait()`, until the queue is
        full. At that point the oldest item already in the queue is discarded,
        to make room for the new item.

        @note
            This method is not thread-safe. There is a chance that one thread
            discards the oldest item just in time to make room for a second
            thread to insert a new one. In this case, the first thread may
            block.
        '''

        try:
            return self.put_nowait(item)

        except Exception: #(queue.Full, asyncio.QueueFull):
            try:
                self.get_nowait()
            except Exception: #(queue.Empty, asyncio.QueueEmpty):
                pass

            return self.put(item)



class Queue (RoundRobin, queue.Queue):
    def __init__(self, *args, **kwargs):
        queue.Queue.__init__(self, *args, **kwargs)
        self._full_queue_mutex = threading.Lock()

    def put_roundrobin(self, item: object):
        with self._full_queue_mutex:
            RoundRobin.put_roundrobin(self, item)


class AsyncQueue (RoundRobin, asyncio.Queue):
    pass


class MultiprocessQueue (RoundRobin, multiprocessing.queues.Queue):
    pass
