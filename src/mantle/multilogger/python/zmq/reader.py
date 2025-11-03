'''
Connect to MultiLogger and monitor for incoming logs
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
import logging


### Modules within this package
from cc.core.roundrobin import Queue
from cc.core.decorators import doc_inherit
from cc.protobuf.status import Level
from cc.messaging.zmq.basic import Subscriber as BaseSubscriber
from cc.messaging.zmq.protobuf import MessageHandler

from ..protobuf import Loggable
from .common import MULTILOGGER_MONITOR_CHANNEL

#-------------------------------------------------------------------------------
# MultiLogger Subscriber

class Subscriber (BaseSubscriber):
    channel_name = MULTILOGGER_MONITOR_CHANNEL


#-------------------------------------------------------------------------------
# MultiLogger Reader

class Reader (MessageHandler):
    '''
    Log handler that submits messages to `Multilogger` over ZMQ
    '''

    def __init__(
            self,
            subscriber: Subscriber|None = None,
            threshold: int|Level = logging.NOTSET,
            queue_size: int = 4096,
    ):
        '''
        Initializer.

        Inputs:
        @param id
            Identity of this listener for logging purposes

        @param threshold
            Minimum capture level

        @param queue_size
            Max. number of log messages to cache locally

        @param overflow_disposition
            Controls behavior when a new message is captured by the capture queue is full.
        '''

        self.queue = Queue(queue_size)
        super().__init__(subscriber = subscriber,
                         message_type = Loggable)

    def __iter__(self):
        return self

    def __next__ (self):
        item = self.queue.get()
        if item is not None:
            return item
        else:
            raise StopIteration

    @doc_inherit
    def handle_proto(self, loggable: Loggable):
        self.queue.put_roundrobin(loggable)


if __name__ == '__main__':
    subscriber = Subscriber()
    reader = Reader(subscriber = subscriber)

    subscriber.initialize()
    logging.info(f"MultiLogger ZMQ reader {reader.id} is alive!")

    for loggable in reader:
        print(loggable)

