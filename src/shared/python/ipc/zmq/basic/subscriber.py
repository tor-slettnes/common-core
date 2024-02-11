#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file subscriber.py
## @brief ZeroMQ subscriber base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .satellite      import Satellite
from .filter         import Filter, Topic
from .messagehandler import MessageHandler
from core.invocation import safe_invoke

### Third-party modules
import zmq

### Standard Python modules
from typing import Callable, Optional, Union
import logging, threading

class Subscriber (Satellite):
    endpoint_type = 'subscriber'

    def __init__(self,
                 host_address: str,
                 channel_name: str = None):

        Satellite.__init__(self, host_address, channel_name, zmq.SUB)
        self.subscriptions = set()
        self.receive_thread = None
        self.keep_receiving = False
        self._mtx           = threading.Lock()

    def __del__(self):
        self.clear()

    def add(self,
            handler: MessageHandler):
        '''Add a message handler with subscribing message filter.'''

        with self._mtx:
            logging.info("Subscriber(%r) adding handler %r with filter %r"%
                         (self.channel_name, handler.id, handler.message_filter))
            self._init_handler(handler)
            self.subscriptions.add(handler)

        self.start_receiving()

    def remove(self,
               handler: MessageHandler):
        '''Remove an existing message handler.'''

        with self._mtx:
            logging.info("Subscriber(%r) removing handler %r"%
                         (self.channel_name, handler.id, handler.message_filter))
            self.subscriptions.discard(handler)
            self._deinit_handler(handler)

    def clear(self):
        '''Remove all existing message handlers'''

        with self._mtx:
            for handler in self.subscriptions:
                self._deinit_handler(handler)
            self.subscriptions.clear()

    def _init_handler(self, handler: MessageHandler):
        handler.initialize()
        self.socket.setsockopt(zmq.SUBSCRIBE, handler.message_filter)

    def _deinit_handler(self, handler: MessageHandler):
        self.socket.setsockopt(zmq.UNSUBCRIBE, handler.message_filter)
        handler.deinitialize()

    def _invoke_handler(self,
                        handler: MessageHandler,
                        data: bytes):

        payload = data[len(handler.message_filter):]

        safe_invoke(handler.handle,
                    (payload,),
                    {},
                    "Subscriber(%r) handler %r"%(self.channel_name, handler.id))

    def _process_message (self, data):
        with self._mtx:
            for handler in self.subscriptions:
                if data.startswith(handler.message_filter):
                    self._invoke_handler(handler, data)
                    break
            else:
                logging.notice("Subscriber %r received message with no matching filter: %s"%
                               (self.channel_name, message))

    def start_receiving (self):
        self.keep_receiving = True
        t = self.receive_thread
        if not t or not t.is_alive():
            t = self.receive_thread = threading.Thread(
                None, self._receive_loop, 'Subscriber loop', daemon=True)
            t.start()

    def stop_receiving (self):
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


