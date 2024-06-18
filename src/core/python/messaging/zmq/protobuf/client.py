#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief ZeroMQ RPC client using ProtoBuf payloads
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .error import Error
from ..basic.requester import Requester
from protobuf.wellknown import Message, MessageType, Empty
import protobuf.rr

class Client (Requester):
    '''ZMQ RPC client using ProtoBuf messages'''

    last_client_id = 0
    interface_name = None

    def __init__(self,
                 host_address  : str,
                 channel_name  : str = None,
                 interface_name: str = None):
        Requester.__init__(self, host_address, channel_name)

        if interface_name:
            self.interface_name = interface_name
        else:
            assert self.interface_name, \
                "ZMQ Client subclass %r needs to set `interface_name` -- see %s"%(
                    type(self).__name__, __file__)

        self.request_id     = 0
        self.client_id      = Client.last_client_id+1
        Client.last_client_id += 1


    def call(self,
             method_name  : str,
             request      : Message|None = None,
             response_type: MessageType = Empty) -> Message:
        '''Invoke a remote method using a ProtoBuf envelope, then wait for and
        return the reply from the server.

        '''

        if request is None:
            request = Empty()

        self.send_protobuf_invocation(method_name, request)
        return self.receive_protobuf_result(response_type)


    def send_protobuf_invocation(self,
                                 method_name  : str,
                                 args         : Message):
        '''Invoke a remote method using a ProtoBuf envelope, then return immediately.
        To reeive the response invoke `receive_protobuf_result()`.
        '''
        params = protobuf.rr.Parameter(serialized_proto = args.SerializeToString())
        self.send_invocation(method_name, params)


    def send_invocation(self,
                        method_name: str,
                        input_param: protobuf.rr.Parameter):

        self.request_id += 1

        req = protobuf.rr.Request(request_id = self.request_id,
                                     client_id  = self.client_id,
                                     interface_name = self.interface_name,
                                     method_name = method_name,
                                     param = input_param)

        self.send_request(req)

    def send_request(self, request: protobuf.rr.Request):
        self.send_bytes(request.SerializeToString())

    def receive_protobuf_result(self, response_type : MessageType) -> Message:
        response_param = self.receive_response_param()
        return response_type.FromString(response_param.serialized_proto)

    def receive_response_param(self) -> protobuf.rr.Parameter:
        reply = self.receive_reply()
        if reply.status.code == protobuf.rr.STATUS_OK:
            return reply.param
        else:
            raise Error(reply.status.code, reply.status.details) from None

    def receive_reply(self) -> protobuf.rr.Reply:
        data = self.receive_bytes()
        return protobuf.rr.Reply.FromString(data)
