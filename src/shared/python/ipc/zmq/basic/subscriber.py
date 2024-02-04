#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file subscriber.py
## @brief ZeroMQ subscriber base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .satellite      import Satellite
from .filter         import Filter, Topic
from core.invocation import safe_invoke

### Third-party modules
import zmq

### Standard Python modules
from typing import Callable
import logging, threading

class Subscriber (Satellite):
    endpoint_type = 'subscriber'

    def __init__(self,
                 host_address: str,
                 channel_name: str = None):

        Satellite.__init__(self, host_address, channel_name, zmq.SUB)
        self.subscriptions = {}
        self.receive_thread = None
        self.keep_receiving = False

    def subscribe(self, callback : Callable[[bytes], None]):
        self.subscribe_with_filter(
            Filter(),
            lambda _filter, _data: callback(_data))

    def subscribe_with_topic(self,
                             topic    : Topic,
                             callback : Callable[[str, bytes], None]):

        f = Filter.create_from_topic(topic)
        self.subscribe_with_filter(
            f,
            lambda _filter, _data: callback(_filter.topic(), _data))


    def subscribe_with_filter(self,
                               filter   : Filter,
                               callback : Callable[[Filter, bytes], None]):
        self.subscriptions[filter] = callback
        self.socket.setsockopt(zmq.SUBSCRIBE, filter)
        self._start_receiving()

    def unsubscribe(self):
        self.unsubscribe_with_filter(Filter())

    def unsubscribe_with_topic(self, topic: str):
        f = Filter.create_from_topic(topic)
        self.unsubscribe_with_filter(f)

    def unsubscribe_with_filter(self, filter: Filter):
        self.subscriptions.pop(filter, None)
        self.socket.setsockopt(zmq.UNSUBCRIBE, filter)

    def _start_receiving (self):
        self.keep_receiving = True
        t = self.receive_thread
        if not t or not t.is_alive():
            t = self.receive_thread = threading.Thread(
                None, self._receive_loop, 'Subscriber loop', daemon=True)
            t.start()


    def _stop_receiving (self):
        self.keep_receiving = False
        t = self.receive_thread
        if t and t.is_alive():
            logging.debug('Waiting for ZMQ subscriber thread')
            t.join()

    def _receive_loop (self):
        while self.keep_receiving:
            data = self.receive_bytes()
            if len(data):
                self._process_message(data)

    def _process_message (self, message):
        self.last_publication = message
        for filter, callback in self.subscriptions.items():
            if message.startswith(filter):
                self._invoke_callback(callback, filter,  message[len(filter):])
                break
        else:
            print("Received data for no subscriber: %r", message)

    def _invoke_callback(self, callback, filter, message):
        safe_invoke(callback, (filter, message), {}, log=logging.warning)
