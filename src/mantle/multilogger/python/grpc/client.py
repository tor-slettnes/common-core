'''
Python client for `MultiLogger` gRPC service
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
import logging
import time

### Modules within package
from cc.core.decorators import doc_inherit
from cc.messaging.grpc import Client as BaseClient
from ..base.api import API
from ..base.threaded_submitter import ThreadedSubmitter


#-------------------------------------------------------------------------------
# MultiLogger Client

class Client (BaseClient, API, ThreadedSubmitter):
    '''
    Client for MultiLogger service.
    '''

    from .multilogger_service_pb2_grpc import MultiLoggerStub as Stub

    def __init__(
            self,
            host                : str = "",
            identity            : str = None,
            capture_python_logs : bool = False,
            log_level           : int = logging.NOTSET,
            wait_for_ready      : bool = False,
            queue_size          : int = 4096,
    ):
        '''
        Initializer.

        Inputs:
        @param host
            Host (resolvable name or IP address) of MultiLogger server

        @param identity
            Identity of this client (e.g. process name)

        @param capture_python_logs
            Register this instance as a Python log handler, which will
            capture messages sent via the `logging` module.

        @param log_level
            Capture threshold for the Python log handler, if any.

        @param wait_for_ready
            If server is unavailable, keep waiting instead of failing.

        @param queue_size
            Max. number of log messages to cache locally if server is
            unavailable, before discarding.

        @param overflow_disposition
            Controls behavior when a new message is captured by the submit queue is full.
        '''

        BaseClient.__init__(
            self,
            host = host,
            wait_for_ready = wait_for_ready)

        API.__init__(
            self,
            rpc = self.stub)

        ThreadedSubmitter.__init__(
            self,
            identity = identity,
            capture_python_logs = capture_python_logs,
            log_level = log_level,
            queue_size = queue_size)


    def initialize(self):
        BaseClient.initialize(self)
        self.open()

    def deinitialize(self):
        self.close()
        BaseClient.deinitialize(self)


    def stream_worker(self, wait_for_ready: bool = True):
        '''
        Stream queued messages to MultiLogger service.
        Runs in its own thread until the queue is deleted (i.e., client is closed)
        '''

        while self.is_writer_open():
            try:
                self.stub.Writer(self.queue_iterator(), wait_for_ready = wait_for_ready)
            except Exception as e:
                if self.is_writer_open():
                    self.logger.debug(
                        "Failed to stream message to MultiLogger service at %s: [%s]; retrying in 2s: %s"%
                        (self.host, type(e).__name__, e))
                    time.sleep(2.0)

    def queue_iterator(self):
        if q := self.queue:
            item = q.get()
            while item is not None:
                yield item
                item = q.get()



if __name__ == '__main__':
    client = Client(capture_python_logs = True)
    client.initialize()
    self.logger.info(f"MultiLogger client {client.identity} is alive!")
