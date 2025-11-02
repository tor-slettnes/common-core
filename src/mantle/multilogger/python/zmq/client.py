'''
Python client for `MultiLogger` ZMQ RPC service
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


### Standard Python modules
from typing import Optional
import logging
import threading
import time


### Modules within package
from cc.core.doc_inherit import doc_inherit
from cc.protobuf.wellknown import Empty
from cc.protobuf.status import Level
from cc.messaging.zmq.protobuf import Client as ProtoBufClient

from ..protobuf import ListenerSpec, \
    SinkID, SinkSpec, SinkSpecs, SinkNames, SinkTypes, FieldNames, \
    AddSinkResult, RemoveSinkResult
from ..base.api import API
from .common import MULTILOGGER_SERVICE_CHANNEL, MULTILOGGER_RPC_INTERFACE
from .writer import Writer
from .reader import Subscriber, Reader


#-------------------------------------------------------------------------------
# MultiLogger Client

class Client (API, Writer):
    '''
    Client for MultiLogger service.
    '''

    class RPCStub (ProtoBufClient):
        '''
        RPC stub that forwards method invocations to the MultiLogger server over
        ZMQ, and returns the response from the server.

        Each service method takes exactly one ProtoBuf message as input to be
        sent to the server, and returns exactly one ProtoBuf message as received
        from the server.

        This closely mimicks the corresponding gRPC service stub, which is in
        turn generated from the `MultiLogger` service definition in
        `multilogger.proto`.  However, in this ZMQ-based implementation the
        method invocations and responses are themselves serialized ProtoBuf
        wrapper messages, namely `Request` and `Reply` defined in
        `request_reply.proto`.
        '''

        channel_name = MULTILOGGER_SERVICE_CHANNEL
        interface_name = MULTILOGGER_RPC_INTERFACE

        def AddSink(self, request: SinkSpec) -> AddSinkResult:
            return self.call("add_sink", request, AddSinkResult)

        def RemoveSink(self, request: SinkID) -> RemoveSinkResult:
            return self.call("remove_sink", request, RemoveSinkResult)

        def GetSink(self, request: SinkID) -> SinkSpec:
            return self.call("get_sink", request, SinkSpec)

        def GetAllSinks(self, request: Empty) -> SinkSpecs:
            return self.call("get_all_sinks", request, SinkSpecs)

        def ListSinks(self, request: Empty) -> SinkNames:
            return self.call("list_sinks", request, SinkNames)

        def ListSinkTypes(self, request: Empty) -> SinkTypes:
            return self.call("list_sink_types", request, SinkTypes)

        def ListMessageFields(self, request: Empty) -> FieldNames:
            return self.call("list_message_fields", request, FieldNames)

        def ListErrorFields(self, request: Empty) -> FieldNames:
            return self.call("list_error_fields", request, FieldNames)


    def __init__(
            self,
            host                : str = "",
            identity            : str = None,
            capture_python_logs : bool = False,
            log_level           : int = logging.NOTSET,
            queue_size          : int = 4096,
    ):
        '''
        Initializer.

        Inputs:
        @param host
            Host (resolvable name or IP address) of MultiLogger server

        @param identity
            Identity of this client (e.g. process name)

        @param capture_python_logs
            Register this instance as a Python log handler, which will
            capture messages sent via the `logging` module.

        @param log_level
            Capture threshold for the Python log handler, if any.

        @param queue_size
            Max. number of log messages to cache locally if server is
            unavailable, before discarding.
        '''

        API.__init__(
            self,
            rpc = Client.RPCStub(host_address = host))

        Writer.__init__(
            self,
            host = host,
            identity = identity,
            capture_python_logs = capture_python_logs,
            log_level = log_level,
            queue_size = queue_size)

        self.subscriber = None
        self.host = host


    def initialize(self):
        self.rpc.initialize()
        Writer.initialize(self)

    def deinitialize(self):
        Writer.deinitialize(self)
        if rpc := self.rpc:
            rpc.deinitialize()

    # @doc_inherit
    def listen(self,
               min_level: Level|int = Level.TRACE,
               contract_id: Optional[str] = None):

        spec = self.build_from_dict(ListenerSpec, locals(),
                                    sink_id = self.identity)

        if self.subscriber is None:
            self.subscriber = Subscriber(self.host)
            self.subscriber.initialize()

        return Reader(subscriber = self.subscriber)


if __name__ == '__main__':
    client = Client(capture_python_logs = True)
    client.initialize()
    logging.info(f"MultiLogger ZMQ client {client.identity} is alive!")
