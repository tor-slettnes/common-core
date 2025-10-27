#!/bin/echo Do not invoke directly.
#===============================================================================
## @file threaded_submitter.py
## @brief Python log handler that submits messages to MultiLogger in dedicated thread
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Standard Python modules
import abc
import enum
import logging
import threading


### Modules within this package
from cc.core.roundrobin import Queue
from ..protobuf import Loggable
from .submitter import Submitter


class ThreadedSubmitter (Submitter):
    '''
    Log handler that submits messages to MultiLogger in dedicated thread
    '''

    def __init__(self,
                 identity: str|None,
                 capture_python_logs: bool = False,
                 log_level: int = logging.NOTSET,
                 queue_size: int = 4096,
                 ):
        '''
        @param queue_size
            Max. number of log messages to cache locally if server is
            unavailable, before discarding.
        '''

        super().__init__(
            identity = identity,
            capture_python_logs = capture_python_logs,
            log_level = log_level)

        self.queue = None
        self.queue_size = queue_size
        self.reset()

    def reset(self):
        self._writer_thread = None

    def open(self):
        '''
        Open a connection to MultiLogger in a dedicated thread.
        '''

        self.open_writer()
        super().open()


    def close(self):
        super().close()
        self.close_writer()


    def open_writer(self, *args, **kwargs):
        '''
        Start streaming messages to MultiLogger.

        This creates a local message queue into which subsequent log messages
        are captured and then continuously streamed to the server.  This differs
        from the default behavior, where individual messages are submitted to
        the server via unary RPC calls.

        See also `close_write_stream()`.
        '''

        if not self.is_writer_open():
            self.queue = Queue(self.queue_size)
            t = self._writer_thread = threading.Thread(
                target = self.stream_worker,
                args = args,
                kwargs = kwargs,
                daemon = True,
            )
            t.start()

    def close_writer(self, wait: bool = False):
        '''
        Close any active writer stream to MultiLogger.
        '''

        q = self.queue
        t = self._writer_thread

        self.queue = None
        self._writer_thread = None

        if q:
            q.put(None)

        if wait and t and t.is_alive():
            t.join()

    def is_writer_open(self) -> bool:
        '''
        Indicate whether the writer thread is active.

        @return
            `True` if a the writer thread is running, `False` otherwise.
        '''

        if t := self._writer_thread:
            return t.is_alive()
        else:
            return False


    def submit(self, message: Loggable):
        '''
        Send an loggable item to the MultiLogger server.

        If we have previously invoked `open()` this message is queued for
        streaming via the gRPC `writer()` method, otherwise it is sent
        immediately via a unary RPC call.
        '''

        if queue := self.queue:
            queue.put_roundrobin(message)


    @abc.abstractmethod
    def stream_worker(self):
        '''
        Stream queued messages to MultiLogger service.
        Runs in its own thread until the queue is deleted (i.e., client is closed)
        '''
