'''
Switch controlled via a remote gRPC service
'''

__all__ = ['RemoteSwitch']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


from ..base.switch import Switch
from cc.protobuf.switchboard import \
    Status, State, InterceptorPhase, ExceptionHandling, \
    Specification, SetSpecificationRequest, \
    Localization, LocalizationMap, Dependency, DependencyMap, \
    AddDependencyRequest, RemoveDependencyRequest, DependencyPolarity, \
    InterceptorSpec, AddInterceptorRequest, RemoveInterceptorRequest, \
    InterceptorInvocation, InterceptorResult, \
    SetAttributesRequest, CulpritsQuery

from cc.protobuf.status import Error
from cc.protobuf.variant import PyValueDict, encodeKeyValueMap

from cc.switchboard.switchboard_pb2_grpc import SwitchboardStub
from cc.core.doc_inherit import doc_inherit

from typing import Optional
from collections.abc import Sequence, Mapping

class RemoteSwitch (Switch):

    def __init__ (self,
                  name: str,
                  client_stub: SwitchboardStub):

        Switch.__init__(self, name)
        self.stub = client_stub


    @doc_inherit
    def set_specification(self,
                          specification: Specification,
                          replace_localizations: bool = False,
                          replace_dependencies: bool = False,
                          replace_interceptors: bool = False,
                          active: Optional[bool] = None,
                          update_state: Optional[bool] = None,
                          ):

        req = SetSpecificationRequest(
            switch_name = self.name,
            spec = specification,
            active = active,
            replace_localizations = replace_localizations,
            replace_dependencies = replace_dependencies,
            replace_interceptors = replace_interceptors,
            update_state = update_state)

        return self.stub.SetSpecification(req).value


    @doc_inherit
    def add_dependency(self,
                       predecessor_name: str,
                       trigger_states: int = State.STATE_SETTLED,
                       polarity: DependencyPolarity = DependencyPolarity.POSITIVE,
                       hard: bool = False,
                       sufficient: bool = False,
                       allow_update: Optional[bool] = None,
                       reevaluate: Optional[bool] = None,
                       ) -> bool:

        req = AddDependencyRequest(
            switch_name = self.name,
            predecessor_name = predecessor_name,
            dependency = Dependency(
                trigger_states = trigger_states,
                polarity = polarity,
                hard = hard,
                sufficient = sufficient),
            allow_update = allow_update,
            reevaluate = reevaluate)

        return self.stub.AddDependency(req).value


    @doc_inherit
    def remove_dependency(self,
                          predecessor_name: str,
                          reevaluate: bool = True,
                          ) -> bool:

        req = cc.protobuf.RemoveDependencyRequest(
            switch_name = self.name,
            predecessor_name = predecessor_name,
            reevaluate = reevaluate)

        return self.stub.RemoveDependency(req).value


    @doc_inherit
    def add_interceptor(self,
                        interceptor_name: str,
                        state_transitions: int,
                        asynchronous: bool = False,
                        rerun: bool = False,
                        on_cancel: ExceptionHandling = ExceptionHandling.EH_DEFAULT,
                        on_error: ExceptionHandling = ExceptionHandling.EH_DEFAULT,
                        immediate: bool = False,
                        ) -> bool:

        req = AddInterceptorRequest(
            switch_name = self.name,
            predecessor_name = predecessor_name,
            spec = InterceptorSpec(
                state_transitions = state_transitions,
                asynchronous = asynchronous,
                rerun = rerun,
                on_cancel = on_cancel,
                on_error = on_error),
            immediate = immediate)

        return self.stub.AddInterceptor(req).value


    @doc_inherit
    def remove_interceptor(self,
                           interceptor_name: str,
                           ) -> bool:

        req = RemoveInterceptorRequest(
            switch_name = self.name,
            interceptor_name = interceptor_name)

        return self.stub.RemoveInterceptor(req).value


    @doc_inherit
    def invoke_interceptor(self,
                           interceptor_name : str,
                           state : Optional[int] = None
                           ) -> Optional[Error]:

        req = InterceptorInvocation(
            switch_name = self.name,
            interceptor_name = interceptor_name,
            state = state)

        result = self.stub.InvokeIntereptor(req)
        return result.error

    @doc_inherit
    def set_target(self,
                   target_state: Optional[State] = None,
                   error: Optional[Error] = None,
                   attributes: Optional[PyValueDict] = None,
                   clear_existing: bool = False,
                   with_interceptors: bool = True,
                   trigger_descendants: bool = True,
                   reevaluate: bool = False,
                   on_cancel: ExceptionHandling = ExceptionHandling.EH_DEFAULT,
                   on_error: ExceptionHandling = ExceptionHandling.EH_DEFAULT,
                   ) -> bool:

        req = SetTargetRequest(
            switch_name = self.name,
            target_state = target_state,
            error = error,
            attributes = encodeKeyValueMap(attributes),
            clear_existing = clear_existing,
            with_interceptors = with_interceptors,
            trigger_descendants = trigger_descendants,
            on_cancel = on_cancel,
            on_error = on_error)

        return self.stub.SetTarget(req).updated


    @doc_inherit
    def set_attributes(self,
                       attributes: Optional[PyValueDict] = None,
                       clear_existing: bool = False):

        req = SetAttributesRequest(
            switch_name = self.name,
            attributes = encodeKeyValueMap(attributes),
            clear_existing = clear_existing)

        return self.stub.SetAttributes(req).updated


    @doc_inherit
    def get_culprits(self,
                     expected_position: bool = True) -> Mapping[str, Status]:

        req = CulpritsQuery(switch_name = self.name,
                            expected = expected_position)

        return self.stub.GetCulprits(req).map
