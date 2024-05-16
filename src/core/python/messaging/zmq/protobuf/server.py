#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file server.py
## @brief ZeroMQ publisher - ProtoBuf flavor
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .error import Error
from .requesthandler import RequestHandler
from ..basic.responder import Responder

### Modules relative to install folder
import cc.protobuf.rr
import cc.protobuf.status
import cc.protobuf.variant

### Third-party modules
import google.protobuf.message
import google.protobuf.text_format

### Standard Python modules
from typing import Sequence
import logging


class Server (Responder):
    '''ZMQ RPC server using ProtoBuf messages'''

    def __init__(self,
                 bind_address    : str,
                 channel_name    : str,
                 request_handlers: Sequence[RequestHandler] = []):

        Responder.__init__(self, bind_address, channel_name)
        self.request_handlers = dict([(handler.interface_name, handler)
                                      for handler in request_handlers])

    def process_request(self, binary_request: bytes):
        reply = cc.protobuf.rr.Reply()

        try:
            request = cc.protobuf.rr.Request.FromString(binary_request)
        except google.protobuf.message.DecodeError as e:
            logging.warning('ZMQ RPC server failed to decode ProtoBuf request: %s'%
                            (binary_request,))

            Error(cc.protobuf.rr.STATUS_INVALID,
                  cc.protobuf.status.Event(
                      text = str(e),
                      symbol = type(e).__name__,
                      flow = cc.protobuf.status.FLOW_CANCELLED,
                      attributes = cc.protobuf.variant.valueList(
                          data = binary_request
                      )
                  )
            ).add_to_reply(reply)

        else:
            logging.debug('ZMQ RPC server handling request: %s'%(
                google.protobuf.text_format.MessageToString(request, as_one_line=True),))
            self.process_protobuf_request(request, reply)

        logging.debug('ZMQ RPC server sending reply: %s'%(
            google.protobuf.text_format.MessageToString(reply, as_one_line=True),))

        return reply.SerializeToString()


    def process_protobuf_request(self,
                                 request : cc.protobuf.rr.Request,
                                 reply   : cc.protobuf.rr.Reply):

        reply.client_id = request.client_id
        reply.request_id = request.request_id

        try:
            handler = self.request_handlers[request.interface_name]
        except KeyError:
            Error(cc.protobuf.rr.STATUS_INVALID,
                  cc.protobuf.status.Event(
                      text = "No such RPC interface",
                      symbol = 'KeyError',
                      flow = cc.protobuf.status.FLOW_CANCELLED,
                      attributes = cc.protobuf.variant.valueList(
                          interface_name = request.interface_name,
                          available_interfaces = list(self.request_handlers.keys())
                      )
                  )
            ).add_to_reply(reply)
        else:
            handler(request, reply)
