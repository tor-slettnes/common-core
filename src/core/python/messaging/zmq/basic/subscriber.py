#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file subscriber.py
## @brief ZeroMQ subscriber base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .endpoint import Endpoint
from .filter import Filter, Topic
from .messagehandler import MessageHandler
from cc.core.invocation import safe_invoke
from cc.core.invocation import caller_frame

### Third-party modules
import zmq

### Standard Python modules
from typing import Callable, Optional, Union
import logging, threading

class Subscriber (Endpoint):
    endpoint_type = 'subscriber'

    def __init__(self,
                 host_address: str,
                 channel_name: str|None = None,
                 project_name: str|None = None,
                 role: Endpoint.Role = Endpoint.Role.SATELLITE,
                 ):

        Endpoint.__init__(
            self,
            address = host_address,
            channel_name = channel_name,
            project_name = project_name,
            socket_type = zmq.SUB,
            role = role)

        self.subscriptions = set()
        self.receive_thread = None
        self.keep_receiving = False
        self._mtx           = threading.Lock()

    def add(self,
            handler: MessageHandler):
        '''
        Add a message handler with subscribing message filter.
        '''

        with self._mtx:
            logging.debug("Subscriber(%r) adding handler %r with filter %r"%
                         (self.channel_name, handler.id, handler.message_filter))
            self._add_handler_filter(handler)
            self.subscriptions.add(handler)


    def remove(self,
               handler: MessageHandler):
        '''
        Remove an existing message handler.
        '''

        with self._mtx:
            logging.debug("Subscriber(%r) removing handler %r"%
                         (self.channel_name, handler.id, handler.message_filter))
            self.subscriptions.discard(handler)
            self._remove_handler_filter(handler)

    def clear(self):
        '''
        Remove all existing message handlers
        '''

        with self._mtx:
            for handler in self.subscriptions:
                self._remove_handler_filter(handler)
            self.subscriptions.clear()

    def initialize(self):
        super().initialize()
        self.start_receiving()

    def deinitialize(self):
        self.stop_receiving()
        super().deinitialize()

    def _add_handler_filter(self, handler: MessageHandler):
        print("Adding handler %r, message filter %r (size %d)"%(
            handler,
            handler.message_filter,
            len(handler.message_filter)))

        self.socket.setsockopt(zmq.SUBSCRIBE, handler.message_filter)

    def _remove_handler_filter(self, handler: MessageHandler):
        self.socket.setsockopt(zmq.UNSUBSCRIBE, handler.message_filter)

    def _invoke_handler(self,
                        handler: MessageHandler,
                        data: bytes):

        payload = data[len(handler.message_filter):]

        safe_invoke(handler.handle,
                    (payload,),
                    {},
                    "Subscriber(%r) handler %r"%(self.channel_name, handler.id))

    def _process_message (self, data):
        found = False
        with self._mtx:
            for handler in self.subscriptions:
                if data.startswith(handler.message_filter):
                    self._invoke_handler(handler, data)
                    found = True

        if not found:
            logging.warning("Subscriber %r received message with no matching filter: %s"%
                            (self.channel_name, data))

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
            if data := self.receive_bytes():
                self._process_message(data)


