'''
Satellite Switchboard implementation that communicates with a Switchboard
service over gRPC.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from abc import abstractmethod
from logging import Logger
from typing import Callable
import re
import sys


### Common Core modules
from cc.core.decorators import override, virtual
from cc.core.invocation import method_path, main_module_path, invoke_maybe_async
from cc.protobuf.status import encodePossibleError
from cc.protobuf.variant import PyValueMap, encodeKeyValueMap
from cc.protobuf.wellknown import BoolValue
from cc.protobuf.utils import message_to_dict
from cc.messaging.grpc import SignalClient

### Switchboard modules
from ..protobuf import (
    AddSwitchRequest, RemoveSwitchRequest, ClearSwitchesRequest,
    SwitchSelectionInput, encodeSwitchSelection, encodeOptionalSwitchSelection,
    ImportRequest, ImportResponse, ExportRequest, ExportResponse,
    SwitchIdentifiers, Status, StatusMap,
    State, StateMask, StateSet, encodeStateSet,
    AddInterceptorRequest, RemoveInterceptorRequest,
    InterceptorInvocation, InterceptorResult, InterceptorMethod,
    InterceptorSpec, InterceptorPhase,
    ExceptionHandling, InvocationStyle,
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
                 logger: Logger|None = None,
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

        SignalClient.__init__(
            self,
            host = host,
            wait_for_ready = wait_for_ready,
            watch_all = watch_all,
            product_name = product_name,
            project_name = project_name,
        )
        SwitchboardBase.__init__(
            self,
            logger = logger)

    def __del__(self):
        self.stop_intercepting()


    def initialize(self):
        SwitchboardBase.initialize(self)
        SignalClient.initialize(self)


    def deinitialize(self):
        SignalClient.deinitialize(self)
        SwitchboardBase.deinitialize(self)


    def _call_add_switch(self,
                         switch_name: str,
                         initially_active: bool):
        request = AddSwitchRequest(
            switch_name = switch_name,
            active = initially_active)
        return self.stub.AddSwitch(request)


    @override
    def get_status(self,
                   selection: SwitchSelectionInput|None = None,
                   with_ancestors: bool = False,
                   ) -> StatusMap:
        request = encodeSwitchSelection(selection, with_ancestors = with_ancestors)
        return self.stub.GetStatuses(request)

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
                invoke_maybe_async(
                    self._call_add_switch,
                    args = (switch_name, initially_active),
                )

            return switch

    @override
    def add_switch(self,
                   switch_name: str,
                   initially_active: bool = False) -> BoolValue:

        with self._switch_lock:
            try:
                switch = self.switches[switch_name]
            except KeyError:
                switch = self.switches[switch_name] = self._new_switch(switch_name)
                switch.status.active = initially_active

        return switch

    @override
    def remove_switch(self,
                      switch_name: str,
                      propagate: bool = True,
                      ) -> BoolValue:
        req = RemoveSwitchRequest(switch_name = switch_name,
                                  propagate = propagate)
        return self.stub.RemoveSwitch(req)

    @override
    def clear_switches(self,
                       reload: bool = False,
                      ) -> BoolValue:
        req = ClearSwitchesRequest(reload = reload)
        return self.stub.ClearSwitches(req)

    @override
    def import_switches(self,
                        declarations: PyValueMap,
                        replace_specifications: bool = False,
                        replace_statuses: bool = False,
                        invoke_interceptors: InvocationStyle = InvocationStyle.INDIRECT,
                        ) -> ImportResponse:

        req = ImportRequest(
            declarations = encodeKeyValueMap(declarations),
            replace_specifications = replace_specifications,
            replace_statuses = replace_statuses,
            invoke_interceptors = invoke_interceptors)

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


    @override
    def add_interceptor(self,
                        interceptor_name: str,
                        switch_selection: SwitchSelectionInput,
                        state_transitions: StateSet,
                        callback: InterceptorMethod,
                        phase: InterceptorPhase = InterceptorPhase.NORMAL,
                        asynchronous: bool = False,
                        rerun: bool = False,
                        on_cancel: ExceptionHandling = ExceptionHandling.ABORT,
                        on_error: ExceptionHandling = ExceptionHandling.FAIL,
                        immediate: bool = False,
                        future: bool = False) -> BoolValue:

        spec = InterceptorSpec(
            state_transitions = encodeStateSet(state_transitions),
            phase = phase,
            asynchronous = asynchronous,
            rerun = rerun,
            on_cancel = on_cancel,
            on_error = on_error,
        )

        request = AddInterceptorRequest(
            interceptor_name = interceptor_name,
            spec = spec,
            switch_selection = encodeSwitchSelection(switch_selection),
            immediate = immediate,
            future = future)

        self.register_interceptor(interceptor_name, callback)
        self.start_intercepting()
        return self.stub.AddInterceptor(request)


    @override
    def remove_interceptor(self,
                           interceptor_name: str,
                           switch_selection: SwitchSelectionInput|None = None,
                           abandon_pending: bool = True,
                           ) -> BoolValue:

        self.deregister_interceptor(interceptor_name, callback)

        request = RemoveInterceptorRequest(
            interceptor_name = interceptor_name,
            switch_selection = encodeOptionalSwitchSelection(switch_selection),
            abandon_pending = abandon_pending)

        return self.stub.RemoveInterceptor(request)


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


    def _enqueue_interceptor_result(self,
                                    queue,
                                    request: InterceptorInvocation,
                                    error: Exception|None = None):
        '''
        Respond back to the server after an interceptor completes.
        '''

        result = InterceptorResult(
            switch_name = request.switch_name,
            interceptor_name = request.interceptor_name,
        )

        if error:
            encodePossibleError(
                error,
                origin = main_module_path(),
                output = result.error,
            )

        queue.put_nowait(result)
