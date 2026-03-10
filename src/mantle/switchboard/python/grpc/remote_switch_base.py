'''
Switch controlled via a remote gRPC service - base
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

from typing import Mapping
from typing import Optional

from cc.core.decorators import override
from cc.protobuf.status import Error, encodePossibleError
from cc.protobuf.variant import PyValueMap, encodeKeyValueMap
from cc.protobuf.wellknown import BoolValue

from ..protobuf import (
    Status, StatusMap, State, StateSet, encodeStateSet,
    InterceptorPhase, ExceptionHandling, InvocationStyle, CascadeStyle,
    SwitchIdentifier, SwitchIdentifiers,
    Specification, SetSpecificationRequest,
    Localization, LocalizationMap, Dependency, DependencyMap,
    AddDependencyRequest, RemoveDependencyRequest, DependencyPolarity,
    InterceptorSpec, InterceptorInvocation, InterceptorResult,
    SetTargetRequest, SetTargetResponse,
    SetAttributesRequest, SetAttributesResponse,
    GetAttributesRequest, GetAttributesResponse,
    CulpritsQuery,
)

from ..base.switch import Switch, InterceptorMethod


class RemoteSwitchBase (Switch):

    def __init__ (self,
                  name: str,
                  client: 'Client'):

        Switch.__init__(self, name, client)
        self.stub = client.stub

    def client(self):
        if board := self.board():
            return board
        else:
            return None

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
    def set_target(self,
                   target_state: Optional[State] = None,
                   error: Error|Exception|str|None = None,
                   attributes: Optional[PyValueMap] = None,
                   clear_existing: bool = False,
                   invoke_interceptors: InvocationStyle = InvocationStyle.ALL,
                   cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                   reenter: bool = False,
                   on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                   on_error: ExceptionHandling = ExceptionHandling.DEFAULT,
                   ) -> SetTargetResponse:

        req = SetTargetRequest(
            switch_name = self.name,
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


    @override
    def get_attributes(self,
                       inherit: bool = False) -> GetAttributesResponse:
        req = GetAttributesRequest(
            switch_name = self.name,
            inherit = inherit)

        return self.stub.GetAttributes(req)

    @override
    def set_attributes(self,
                       attributes: Optional[PyValueMap] = None,
                       clear_existing: bool = False) -> SetAttributesResponse:

        req = SetAttributesRequest(
            switch_name = self.name,
            attributes = encodeKeyValueMap(attributes),
            clear_existing = clear_existing)

        return self.stub.SetAttributes(req)


    @override
    def get_status(self) -> StatusMap:
        req = SwitchIdentifiers(
            switch_names = [self.name])

        return self.stub.GetStatuses(req)


    @override
    def get_culprits(self,
                     expected_position: bool = True) -> StatusMap:

        req = CulpritsQuery(switch_name = self.name,
                            expected = expected_position)

        return self.stub.GetCulprits(req)

    @override
    def get_errors(self) -> Mapping[str, Error]:
        req = SwitchIdentifier(switch_name = self.name)
        return self.stub.GetErrors(req)
