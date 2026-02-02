'''
Switch controlled via a remote gRPC service - base
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

from typing import Mapping
from typing import Optional
from weakref import ref

from cc.core.decorators import override
from cc.protobuf.status import Error, encodeError
from cc.protobuf.variant import PyValueDict, encodeKeyValueMap
from cc.protobuf.wellknown import BoolValue

from ..protobuf import (
    Status, StatusMap, State, StateSet, encodeStateSet,
    InterceptorPhase, ExceptionHandling,
    Specification, SetSpecificationRequest,
    Localization, LocalizationMap, Dependency, DependencyMap,
    AddDependencyRequest, RemoveDependencyRequest, DependencyPolarity,
    InterceptorUpdate, InterceptorRegistration, InterceptorDeregistration,
    InterceptorSpec, InterceptorInvocation, InterceptorResult,
    SetTargetRequest, SetTargetResponse,
    SetAttributesRequest, SetAttributesResponse, CulpritsQuery,
)

from ..base.switch import Switch, InterceptorMethod


class RemoteSwitchBase (Switch):

    def __init__ (self,
                  name: str,
                  client: 'Client'):

        Switch.__init__(self, name = name)
        self.stub = client.stub
        self.client = ref(client)

    @override
    def set_specification(self,
                          specification: Specification,
                          replace_aliases: bool = False,
                          replace_localizations: bool = False,
                          replace_dependencies: bool = False,
                          replace_interceptors: bool = False,
                          active: Optional[bool] = None,
                          update_state: Optional[bool] = None,
                          ) -> BoolValue:

        req = SetSpecificationRequest(
            switch_name = self.name,
            spec = specification,
            active = active,
            replace_aliases = replace_aliases,
            replace_localizations = replace_localizations,
            replace_dependencies = replace_dependencies,
            replace_interceptors = replace_interceptors,
            update_state = update_state)

        return self.stub.SetSpecification(req)


    @override
    def add_dependency(self,
                       predecessor: str,
                       trigger_states: StateSet = State.SETTLED,
                       polarity: DependencyPolarity = DependencyPolarity.POSITIVE,
                       hard: bool = False,
                       sufficient: bool = False,
                       allow_update: Optional[bool] = None,
                       reevaluate: Optional[bool] = None,
                       ) -> BoolValue:

        if isinstance(predecessor, Switch):
            predecessor = predecessor.name

        req = AddDependencyRequest(
            switch_name = self.name,
            predecessor_name = predecessor,
            dependency = Dependency(
                trigger_states = encodeStateSet(trigger_states),
                polarity = polarity,
                hard = hard,
                sufficient = sufficient),
            allow_update = allow_update,
            reevaluate = reevaluate)

        return self.stub.AddDependency(req)


    @override
    def remove_dependency(self,
                          predecessor: str,
                          reevaluate: bool = True,
                          ) -> BoolValue:

        if isinstance(predecessor, Switch):
            predecessor = predecessor.name

        req = RemoveDependencyRequest(
            switch_name = self.name,
            predecessor_name = predecessor,
            reevaluate = reevaluate)

        return self.stub.RemoveDependency(req)


    @override
    def add_interceptor(self,
                        interceptor_name: str,
                        state_transitions: StateSet,
                        callback: InterceptorMethod,
                        phase: InterceptorPhase = InterceptorPhase.NORMAL,
                        asynchronous: bool = False,
                        rerun: bool = False,
                        on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                        on_error: ExceptionHandling = ExceptionHandling.DEFAULT,
                        immediate: bool = False,
                        ):

        is_new = Switch.add_interceptor(**locals())

        if client := self.client():
            spec = InterceptorSpec(
                state_transitions = encodeStateSet(state_transitions),
                asynchronous = asynchronous,
                phase = phase,
                rerun = rerun,
                on_cancel = on_cancel,
                on_error = on_error,
            )

            registration = InterceptorRegistration(
                spec = spec,
                immediate = immediate,
            )

            update = InterceptorUpdate(
                switch_name = self.name,
                interceptor_name = interceptor_name,
                registration = registration,
            )

            client.enqueue_interceptor_update(update)

        return is_new


    @override
    def remove_interceptor(self,
                           interceptor_name: str,
                           ) -> bool:

        if client := self.client():
            update = InterceptorUpdate(
                switch_name = self.name,
                interceptor_name = interceptor_name,
                deregistration = InterceptorDeregistration(),
            )
            client.enqueue_interceptor_update(update)

        return Switch.remove_interceptor(self, interceptor_name)


    @override
    def invoke_interceptor(self,
                           interceptor_name : str,
                           state : Optional[int] = None
                           ) -> InterceptorResult:

        req = InterceptorInvocation(
            switch_name = self.name,
            interceptor_name = interceptor_name,
            state = state)

        return self.stub.InvokeInterceptor(req)


    @override
    def on_intercept(self,
                     interceptor_name : str,
                     state : State):

        interceptor = self.interceptor_methods[interceptor_name]
        return interceptor(self, interceptor_name, state)

    @override
    def set_target(self,
                   target_state: Optional[State] = None,
                   error: Error|Exception|str|None = None,
                   attributes: Optional[PyValueDict] = None,
                   clear_existing: bool = False,
                   with_interceptors: bool = True,
                   trigger_descendants: bool = True,
                   reevaluate: bool = False,
                   on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                   on_error: ExceptionHandling = ExceptionHandling.DEFAULT,
                   ) -> SetTargetResponse:

        req = SetTargetRequest(
            switch_name = self.name,
            target_state = target_state,
            error = None if error is None else encodeError(error),
            attributes = encodeKeyValueMap(attributes),
            clear_existing = clear_existing,
            with_interceptors = with_interceptors,
            trigger_descendants = trigger_descendants,
            on_cancel = on_cancel,
            on_error = on_error)

        return self.stub.SetTarget(req)


    @override
    def set_attributes(self,
                       attributes: Optional[PyValueDict] = None,
                       clear_existing: bool = False) -> SetAttributesResponse:

        req = SetAttributesRequest(
            switch_name = self.name,
            attributes = encodeKeyValueMap(attributes),
            clear_existing = clear_existing)

        return self.stub.SetAttributes(req)


    @override
    def get_culprits(self,
                     expected_position: bool = True) -> StatusMap:

        req = CulpritsQuery(switch_name = self.name,
                            expected = expected_position)

        return self.stub.GetCulprits(req)


