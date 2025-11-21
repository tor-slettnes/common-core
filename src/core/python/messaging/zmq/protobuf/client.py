#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief ZeroMQ RPC client using ProtoBuf payloads
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .error import Error
from ..basic.endpoint import Endpoint
from ..basic.requester import Requester
from cc.protobuf.wellknown import Message, MessageType, Empty
from cc.protobuf.request_reply import Parameter, Request, Reply, StatusCode

class Client (Requester):
    '''
    ZMQ RPC client using ProtoBuf messages
    '''

    last_client_id = 0
    interface_name = None

    def __init__(self,
                 host_address  : str,
                 channel_name  : str|None = None,
                 product_name  : str|None = None,
                 interface_name: str|None = None,
                 role          : Endpoint.Role = Endpoint.Role.SATELLITE,
                 ):

        Requester.__init__(
            self,
            host_address = host_address,
            channel_name = channel_name,
            product_name = product_name,
            role = role,
        )

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
        '''
        Invoke a remote method using a ProtoBuf envelope, then wait for and
        return the reply from the server.
        '''

        if request is None:
            request = Empty()

        self.send_protobuf_invocation(method_name, request)
        return self.receive_protobuf_result(response_type)


    def send_protobuf_invocation(self,
                                 method_name  : str,
                                 args         : Message):
        '''
        Invoke a remote method using a ProtoBuf envelope, then return
        immediately.  To reeive the response invoke `receive_protobuf_result()`.
        '''
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

    def receive_protobuf_result(self, response_type : MessageType) -> Message:
        response_param = self.receive_response_param()
        return response_type.FromString(response_param.serialized_proto)

    def receive_response_param(self) -> Parameter:
        reply = self.receive_reply()
        if reply.status.code == StatusCode.OK:
            return reply.param
        else:
            raise Error(reply.status.code, reply.status.details) from None

    def receive_reply(self) -> Reply:
        data = self.receive_bytes()
        return Reply.FromString(data)
