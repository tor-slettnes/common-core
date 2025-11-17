#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file subscriber.py
## @brief ZeroMQ subscriber base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .endpoint import Endpoint
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
                 product_name: str|None = None,
                 role: Endpoint.Role = Endpoint.Role.SATELLITE,
                 ):

        Endpoint.__init__(
            self,
            address = host_address,
            channel_name = channel_name,
            product_name = product_name,
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
            self.logger.debug("Subscriber(%r) adding handler %r with filter %r"%
                         (self.channel_name, handler.id, handler.message_filter))
            self._add_handler_filter(handler)
            self.subscriptions.add(handler)


    def remove(self,
               handler: MessageHandler):
        '''
        Remove an existing message handler.
        '''

        with self._mtx:
            self.logger.debug("Subscriber(%r) removing handler %r"%
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
        self.logger.debug(
            "Adding handler %r, message filter %r (size %d)"%
            (handler, handler.message_filter, len(handler.message_filter)))

        self.socket.setsockopt(zmq.SUBSCRIBE, handler.message_filter)

    def _remove_handler_filter(self, handler: MessageHandler):
        self.socket.setsockopt(zmq.UNSUBSCRIBE, handler.message_filter)

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
            self.logger.debug('Waiting for ZMQ subscriber thread')
            t.join()

    def _receive_loop (self):
        while self.keep_receiving:
            if parts := self.receive_parts():
                self._process_message(parts)

    def _process_message (self, parts: list[bytes]):
        found = False
        with self._mtx:
            for handler in self.subscriptions:
                if (handler.message_filter is None) or (handler.message_filter == parts[0]):
                    self._invoke_handler(handler, parts)
                    found = True

        if not found:
            self.logger.warning("Subscriber %r received message with no matching filter: %s"%
                            (self.channel_name, parts))

    def _invoke_handler(self,
                        handler: MessageHandler,
                        parts: list[bytes]):

        safe_invoke(handler.handle_parts,
                    (parts,),
                    {},
                    "Subscriber(%r) handler %r"%(self.channel_name, handler.id))
