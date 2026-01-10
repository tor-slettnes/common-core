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

    def __iter__(self):
        '''Support iterator protocol'''
        return self

    def next(self):
        return self.get()

    def put_roundrobin (self, item: object):
        '''
        Put an item in the queue.

        This behaves similar to `put_nowait()`, until the queue is full.  At
        that point the oldest item already in the queue is discarded, to make
        room for the new item.
        '''

        ### We may need to try multiple times in case there are other
        ### threads/tasks waiting to put items in this queue.
        sent = False
        while not sent:
            try:
                return self.put_nowait(item)
            except Exception: #(queue.Full, asyncio.QueueFull):
                try:
                    self.get_nowait()
                except Exception: #(queue.Empty, asyncio.QueueEmpty):
                    pass
            else:
                sent = True


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
