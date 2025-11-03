'''
Log handler that submits messages to `MultiLogger` over ZMQ
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


### Standard Python modules
import logging
import time
import sys

### Modules within package
from cc.core.decorators import doc_inherit
from cc.messaging.zmq.basic import Endpoint, Publisher
from cc.messaging.zmq.protobuf import MessageWriter

from ..protobuf import Loggable
from ..base.threaded_submitter import ThreadedSubmitter
from .common import MULTILOGGER_SUBMIT_CHANNEL

#-------------------------------------------------------------------------------
# MultiLogger Writer

class Writer (ThreadedSubmitter, MessageWriter):
    '''
    Log handler that submits messages to `Multilogger` over ZMQ
    '''

    message_type = Loggable

    def __init__(
            self,
            host                : str = "",
            identity            : str = None,
            capture_python_logs : bool = False,
            log_level           : int = logging.NOTSET,
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

        @param queue_size
            Max. number of log messages to cache locally if server is
            unavailable, before discarding.
        '''

        publisher = Publisher(
            host,
            role = Endpoint.Role.SATELLITE,
            channel_name = MULTILOGGER_SUBMIT_CHANNEL)

        MessageWriter.__init__(
            self,
            publisher = publisher)

        ThreadedSubmitter.__init__(
            self,
            identity = identity,
            capture_python_logs = capture_python_logs,
            log_level = log_level,
            queue_size = queue_size)


    def initialize(self):
        self.publisher.initialize()
        MessageWriter.initialize(self)
        ThreadedSubmitter.open(self)

    def deinitialize(self):
        ThreadedSubmitter.close(self)
        MessageWriter.deinitialize(self)
        self.publisher.deinitialize()


    def stream_worker(self):
        '''
        Stream queued messages to MultiLogger service.
        Runs in its own thread until the queue is deleted (i.e., client is closed)
        '''

        while q := self.queue:
            try:
                while loggable := q.get():
                    sys.stdout.write("Got item from write queue: %s"%(loggable,))
                    self.write_proto(loggable)

            except Exception as e:
                if self.queue:
                    logging.debug(
                        "Failed to stream message to MultiLogger service at %s: [%s]; retrying in 2s: %s"%
                        (self.host, type(e).__name__, e))
                    time.sleep(2.0)


if __name__ == '__main__':
    submitter = Writer(capture_python_logs = True)
    submitter.initialize()
    logging.info(f"MultiLogger ZMQ submitter {client.identity} is alive!")
