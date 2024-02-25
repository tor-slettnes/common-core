#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief ZeroMQ RPC client using ProtoBuf payloads
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from .error import Error
from ..basic.requester import Requester
from cc.io.protobuf import CC, ProtoBuf
from generated.request_reply_pb2 import Request, Reply, Parameter, \
    StatusCode, Status, STATUS_OK

from typing import Optional

class Client (Requester, ProtoBuf):
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
             request_args : Optional[ProtoBuf.Message] = None,
             response_type: ProtoBuf.MessageType = ProtoBuf.Empty) -> ProtoBuf.Message:
        '''
        Invoke a remote method using a ProtoBuf envelope
        '''

        if request_args is None:
            request_args = ProtoBuf.Empty()

        self.send_protobuf_invocation(method_name, request_args)
        return self.receive_protobuf_result(response_type)


    def send_protobuf_invocation(self,
                                 method_name  : str,
                                 args         : ProtoBuf.Message):

        params = Parameter(serialized_proto = args.SerializeToString())
        self.send_invocation(method_name, params)


    def send_invocation(self,
                        method_name: str,
                        input_param: Parameter):

        self.request_id += 1

        req = Request(request_id = self.request_id,
                      client_id  = self.client_id,
                      interface_name = self.interface_name,
                      method_name = method_name,
                      param = input_param)

        self.send_request(req)

    def send_request(self, request: Request):
        self.send_bytes(request.SerializeToString())

    def receive_protobuf_result(self, response_type : ProtoBuf.MessageType):
        response_param = self.receive_response_param()
        return response_type.FromString(response_param.serialized_proto)

    def receive_response_param(self):
        reply = self.receive_reply()
        if reply.status.code == STATUS_OK:
            return reply.param
        else:
            raise Error(reply.status.code, reply.status.details) from None

    def receive_reply(self):
        data = self.receive_bytes()
        return Reply.FromString(data)
