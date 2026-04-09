'''
Satellite Switchboard implementation that communicates with a Switchboard
service over gRPC.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from abc import abstractmethod
from logging import Logger
from typing import Callable, Sequence
import re
import sys


### Common Core modules
from cc.core.decorators import override, virtual
from cc.core.invocation import method_path, main_module_path
from cc.protobuf.status import encodePossibleError, Error
from cc.protobuf.variant import PyValueMap, KeyValueMap, encodeKeyValueMap
from cc.protobuf.wellknown import Message, BoolValue
from cc.messaging.grpc import SignalClient

### Switchboard modules
from ..protobuf import (
    AddSwitchRequest, RemoveSwitchRequest, ClearSwitchesRequest,
    SwitchSelectionInput, encodeSwitchSelection, encodeOptionalSwitchSelection,
    ImportRequest, ImportResponse, ExportRequest, ExportResponse,
    SwitchIdentifier, SwitchIdentifiers,
    SetTargetRequest, SetTargetResponse,
    SwitchMap, Specification, SpecificationMap, SetSpecificationRequest,
    Dependency, DependencyStatus, DependencyStatusMap, DependencyPolarity,
    AddDependencyRequest, RemoveDependencyRequest,
    SetAttributesRequest, SetAttributesResponse,
    GetAttributesRequest, GetAttributesResponse,
    Status, StatusMap, CulpritsQuery, ErrorMap,
    State, StateMask, StateSet, encodeStateSet,
    AddInterceptorRequest, RemoveInterceptorRequest,
    InterceptorInvocation, InterceptorResult, InterceptorMethod,
    InterceptorSpec, InterceptorPhase,
    ExceptionHandling, InvocationStyle, CascadeStyle,
)

from ..base.baseboard import SwitchboardBase, Switch

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

    def _get_or_add_switch_proxy(self,
                                 switch_name: str,
                                 initially_active: bool = False) -> tuple[Switch, bool]:

        try:
            return self.switches[switch_name], False

        except KeyError:
            switch = self.switches[switch_name] = self._new_switch(switch_name)
            switch.status.active = initially_active
            return switch, True

    def call_add_switch(self,
                        switch_name: str,
                        initially_active: bool) -> bool:

        request = AddSwitchRequest(
            switch_name = switch_name,
            active = initially_active)
        return self.stub.AddSwitch(request)


    def call_remove_switch(self,
                           switch_name: str,
                           propagate: bool = True,
                           ) -> BoolValue:
        req = RemoveSwitchRequest(switch_name = switch_name,
                                  propagate = propagate)
        return self.stub.RemoveSwitch(req)

    def call_clear_switches(self,
                            reload: bool = False,
                            ) -> BoolValue:
        req = ClearSwitchesRequest(reload = reload)
        return self.stub.ClearSwitches(req)

    def call_import_switches(self,
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

    def call_export_switches(self,
                             selection: SwitchSelectionInput|None = None,
                             include_specifications: bool = False,
                             include_statuses: bool = True) -> ExportResponse:

        req = ExportRequest(
            selection = encodeOptionalSwitchSelection(selection),
            include_specifications = include_specifications,
            include_statuses = include_statuses)

        return self.stub.ExportSwitches(req)

    def call_get_switch_info(self,
                             selection: SwitchSelectionInput|None = None,
                             with_ancestors: bool = False,
                             ) -> SwitchMap:

        request = encodeOptionalSwitchSelection(
            selection,
            with_ancestors = with_ancestors)

        return self.stub.GetSwitches(request)

    def call_set_specification(self,
                               switch_name: str,
                               specification: Specification,
                               replace_aliases: bool = False,
                               replace_localizations: bool = False,
                               replace_dependencies: bool = False,
                               replace_interceptors: bool = False,
                               active: bool|None = None,
                               update_state: bool|None = None) -> BoolValue:

        req = SetSpecificationRequest(
            switch_name = switch_name,
            spec = specification,
            update_state=update_state,
            replace_aliases = replace_aliases,
            replace_localizations = replace_localizations,
            replace_dependencies = replace_dependencies,
            replace_interceptors = replace_interceptors)

        return self.stub.SetSpecification(req)

    def call_get_specifications(self,
                                selection: SwitchSelectionInput|None = None,
                                with_ancestors: bool = False,
                                ) -> SpecificationMap:

        request = encodeOptionalSwitchSelection(
            selection,
            with_ancestors = with_ancestors)

        return self.stub.GetSpecifications(request)


    def call_add_dependency(self,
                            switch_name: str,
                            predecessor_name: str,
                            trigger_states: StateSet = State.SETTLED,
                            polarity: DependencyPolarity = DependencyPolarity.POSITIVE,
                            hard: bool = False,
                            sufficient: bool = False,
                            allow_update: bool|None = None,
                            reevaluate: bool|None = None,
                            ) -> BoolValue:

        req = AddDependencyRequest(
            switch_name = switch_name,
            predecessor_name = predecessor_name,
            dependency = Dependency(
                trigger_states = encodeStateSet(trigger_states),
                polarity = polarity,
                hard = hard,
                sufficient = sufficient),
            allow_update = allow_update,
            reevaluate = reevaluate)

        return self.stub.AddDependency(req)

    def call_remove_dependency(self,
                               switch_name: str,
                               predecessor_name: str,
                               reevaluate: bool = True,
                               ) -> BoolValue:

        req = RemoveDependencyRequest(
            switch_name = switch_name,
            predecessor_name = predecessor_name,
            reevaluate = reevaluate)

        return self.stub.RemoveDependency(req)

    def call_get_ancestors(self,
                           switch_name: str) -> Sequence[str]:

        request = SwitchIdentifier(switch_name = switch_name)
        return self.stub.GetAncestors(request)

    def call_get_descendants(self,
                             switch_name: str) -> Sequence[str]:

        request = SwitchIdentifier(switch_name = switch_name)
        return self.stub.GetAncestors(request)


    def call_set_target(self,
                        switch_name: str,
                        target_state: State|None = None,
                        error: Error|Exception|str|None = None,
                        attributes: PyValueMap|None = None,
                        clear_existing: bool = False,
                        invoke_interceptors: InvocationStyle = InvocationStyle.ALL,
                        cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                        reenter: bool = False,
                        on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                        on_error: ExceptionHandling = ExceptionHandling.DEFAULT,
                        ) -> SetTargetResponse:

        req = SetTargetRequest(
            switch_name = switch_name,
            target_state = target_state,
            attributes = encodeKeyValueMap(attributes),
            error = encodePossibleError(error),
            clear_existing = clear_existing,
            invoke_interceptors = invoke_interceptors,
            cascade_descendants = cascade_descendants,
            reenter = reenter,
            on_cancel = on_cancel,
            on_error = on_error)

        return self.stub.SetTarget(req)

    def call_get_statuses(self,
                          selection: SwitchSelectionInput|None = None,
                          with_ancestors: bool = False,
                          ) -> StatusMap:

        request = encodeOptionalSwitchSelection(
            selection,
            with_ancestors = with_ancestors)

        return self.stub.GetStatuses(request)


    def call_get_dependency_status(self,
                                   switch_name: str,
                                   ) -> DependencyStatusMap:
        req = SwitchIdentifier(switch_name = switch_name)
        return self.stub.GetDependencyStatuses(req)

    def call_set_attributes(self,
                            switch_name: str,
                            attributes: PyValueMap|None = None,
                            clear_existing: bool = False) -> SetAttributesResponse:

        req = SetAttributesRequest(
            switch_name = switch_name,
            attributes = encodeKeyValueMap(attributes),
            clear_existing = clear_existing)

        return self.stub.SetAttributes(req)

    def call_get_attributes(self,
                            switch_name: str,
                            inherit: bool = False) -> GetAttributesResponse:
        req = GetAttributesRequest(
            switch_name = switch_name,
            inherit = inherit)

        return self.stub.GetAttributes(req)

    def call_get_culprits(self,
                          switch_name: str,
                          expected_position: bool = True) -> StatusMap:

        return self.stub.GetCulprits(CulpritsQuery(
            switch_name = switch_name,
            expected = expected_position))


    def call_get_errors(self,
                        switch_name: str,
                        ) -> ErrorMap:

        req = SwitchIdentifier(switch_name = switch_name)
        return self.stub.GetErrors(req)


    def call_invoke_interceptor(self,
                                interceptor_name: str,
                                switch_name: str,
                                state: State|None = None
                                ) -> Error|None:

        req = InterceptorInvocation(
            interceptor_name = interceptor_name,
            switch_name = switch_name,
            state = state)

        return self.stub.InvokeInterceptor(req)


    def call_add_interceptor(self,
                             interceptor_name: str,
                             switch_selection: SwitchSelectionInput,
                             state_transitions: StateSet,
                             callback: InterceptorMethod,
                             phase: InterceptorPhase = InterceptorPhase.NORMAL,
                             asynchronous: bool = False,
                             rerun: bool = False,
                             immediate: bool = False,
                             future: bool = False,
                             on_cancel: ExceptionHandling = ExceptionHandling.ABORT,
                             on_error: ExceptionHandling = ExceptionHandling.FAIL,
                             ) -> BoolValue:

        SwitchboardBase.add_interceptor(**locals())

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

        return self.stub.AddInterceptor(request)


    def call_remove_interceptor(self,
                                interceptor_name: str,
                                switch_selection: SwitchSelectionInput|None = None,
                                abandon_pending: bool = True,
                                ) -> BoolValue:

        SwitchboardBase.remove_interceptor(**locals())

        request = RemoveInterceptorRequest(
            interceptor_name = interceptor_name,
            switch_selection = encodeOptionalSwitchSelection(switch_selection),
            abandon_pending = abandon_pending)

        return self.stub.RemoveInterceptor(request)


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
