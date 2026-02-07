'''
Satellite Switchboard implementation that communicates with a Switchboard
service over gRPC.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from abc import abstractmethod
import re
import sys


### Common Core modules
from cc.core.decorators import override, virtual
from cc.core.invocation import method_path, invoke_maybe_async
from cc.protobuf.status import encodeError
from cc.protobuf.variant import PyValueMap, encodeKeyValueMap
from cc.protobuf.wellknown import BoolValue
from cc.messaging.grpc import SignalClient

### Switchboard modules
from ..protobuf import (
    AddSwitchRequest, RemoveSwitchRequest,
    ImportRequest, ImportResponse, ExportRequest, ExportResponse,
    State, InterceptorInvocation, InterceptorResult, InterceptorUpdate,
    SwitchSelectionInput, encodeSwitchSelection
)
from ..base.baseboard import SwitchboardBase
from .remote_switch import RemoteSwitch


class BaseClient (SwitchboardBase, SignalClient):
    '''
    Switchboard abstract gRPC base client.

    Methods in this module are simple wrappers around corresponding gRPC calls,
    whose responses are returned unmodified. Depending on the gRPC channel type,
    this may be an `asyncio` coroutine that must be awaited.

    Likely you will not use this module directly, but rather one of the derived
    classes `Client` or `AsyncClient`.
    '''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `cc.messaging.grpc.GenericClient` base to instantiate `self.stub`.
    from .switchboard_service_pb2_grpc import SwitchboardStub as Stub

    def __init__(self,
                 host: str = "",
                 wait_for_ready: bool = True,
                 watch_all: bool = True,
                 product_name: str|None = None,
                 project_name: str|None = None,
                 ):
        '''
        @param host
            IP address or resolvable host name of platform server

        @param product_name
            Name of the product, used to locate corresponding settings files
            (e.g. `grpc-endpoints-PRODUCT.yaml`).

        @param project_name
            Name of code project (e.g. parent code repository). Used to locate
            corresponding settings files (e.g., `grpc-endpoints-PROJECT.yaml`)
        '''

        SwitchboardBase.__init__(self)
        SignalClient.__init__(
            self,
            host = host,
            wait_for_ready = wait_for_ready,
            watch_all = watch_all,
            product_name = product_name,
            project_name = project_name,
        )

        self.init_intercept()

    def __del__(self):
        self.stop_intercepting()

    @override
    def get_or_add_switch(self,
                          switch_name: str,
                          initially_active: bool = False,
                          ) -> RemoteSwitch:

        with self._switch_lock:
            try:
                switch = self.switches[switch_name]
            except KeyError:
                switch = self.switches[switch_name] = self._new_switch(switch_name)
                switch.status.active = initially_active
                invoke_maybe_async(self.add_switch,
                                   args = (switch_name, initially_active))

            return switch

    @override
    def add_switch(self,
                   switch_name: str,
                   active: bool = False) -> BoolValue:
        req = AddSwitchRequest(
            switch_name = switch_name,
            active = active)
        return self.stub.AddSwitch(req)

    @override
    def remove_switch(self,
                      switch_name: str,
                      propagate: bool = True,
                      ) -> BoolValue:
        req = RemoveSwitchRequest(switch_name = switch_name,
                                  propagate = propagate)
        return self.stub.RemoveSwitch(req)

    @override
    def import_switches(self,
                        declarations: PyValueMap,
                        replace_specifications: bool = False,
                        replace_statuses: bool = False) -> ImportResponse:

        req = ImportRequest(
            declarations = encodeKeyValueMap(declarations),
            replace_specifications = replace_specifications,
            replace_statuses = replace_statuses)

        return self.stub.ImportSwitches(req)

    @override
    def export_switches(self,
                        selection: SwitchSelectionInput|None = None,
                        include_specifications: bool = False,
                        include_statuses: bool = True) -> ExportResponse:

        req = ExportRequest(
            selection = encodeSwitchSelection(selection),
            include_specifications = include_specifications,
            include_statuses = include_statuses)

        return self.stub.ExportSwitches(req)


    @abstractmethod
    def init_intercept(self):
        '''
        Initialize interception handling.
        '''

    @abstractmethod
    def is_intercepting(self) -> bool:
        '''
        Indicate whether the the client is operating any interceptors.
        '''

    @abstractmethod
    def start_intercepting(self):
        '''
        Start a worker task to handle Interceptor invocations from the
        server. Does nothing if the task is already running.
        '''

    @abstractmethod
    def stop_intercepting(self, wait=True):
        '''
        Stop any running worker task to handle Interceptor invocations from
        the server.

        @param wait
            Wait for the worker task to finish before returning.
        '''

    @abstractmethod
    def enqueue_interceptor_update(self, msg: InterceptorUpdate):
        '''
        Enqueue and send an interceptor update to the Switchboard service.
        '''


    def _return_interceptor_response(self,
                                     request: InterceptorInvocation,
                                     error: Exception|None = None):
        '''
        Enqueue a response back to the server after an interceptor
        completes.
        '''

        result = InterceptorResult()

        if error:
            self.logger.error("%s switch %r interceptor %r failed: [%s] %s" % (
                self,
                request.switch_name,
                request.interceptor_name,
                type(e).__name__,
                e
            ))
            encodeError(
                e,
                origin = sys.modules['__main__'].__spec__.name,
                output = result.error,
            )

        self.enqueue_interceptor_update(
            InterceptorUpdate(
                switch_name = request.switch_name,
                interceptor_name = request.interceptor_name,
                invocation_result = result,
        ))


