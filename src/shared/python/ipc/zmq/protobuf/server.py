#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file server.py
## @brief ZeroMQ publisher - ProtoBuf flavor
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from .error             import Error
from .requesthandler    import RequestHandler
from ..basic.responder  import Responder
from ...google.protobuf import CC, ProtoBuf
from typing             import Sequence

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
        reply = CC.RR.Reply()

        try:
            request = CC.RR.Request.FromString(binary_request)
        except ProtoBuf.DecodeError as e:
            logging.warning('ZMQ RPC server failed to decode ProtoBuf request: %s'%
                            (binary_request,))

            Error(CC.RR.STATUS_INVALID,
                  CC.Status.Details(
                      text = str(e),
                      symbol = type(e).__name__,
                      flow = CC.Status.FLOW_CANCELLED,
                      attributes = CC.valueList(
                          data = binary_request
                      )
                  )
            ).add_to_reply(reply)

        else:
            logging.debug('ZMQ RPC server handling request: %s'%(
                ProtoBuf.MessageToString(request, as_one_line=True),))
            self.process_protobuf_request(request, reply)

        logging.debug('ZMQ RPC server sending reply: %s'%(
            ProtoBuf.MessageToString(reply, as_one_line=True),))

        return reply.SerializeToString()


    def process_protobuf_request(self,
                                 request : CC.RR.Request,
                                 reply   : CC.RR.Reply):

        reply.client_id = request.client_id
        reply.request_id = request.request_id

        try:
            handler = self.request_handlers[request.interface_name]
        except KeyError:
            Error(CC.RR.STATUS_INVALID,
                  CC.Status.Details(
                      text = "No such RPC interface",
                      symbol = 'KeyError',
                      flow = CC.Status.FLOW_CANCELLED,
                      attributes = CC.valueList(
                          interface_name = request.interface_name,
                          available_interfaces = list(self.request_handlers.keys())
                      )
                  )
            ).add_to_reply(reply)
        else:
            handler(request, reply)
