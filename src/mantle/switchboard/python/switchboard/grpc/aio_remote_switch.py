'''
Switch controlled via a remote gRPC service - AsyncIO flavor.
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

from cc.generated.switchboard_pb2_grpc import SwitchboardStub
from cc.core.doc_inherit import doc_inherit

from typing import Optional
from collections.abc import Sequence, Mapping

class AsyncRemoteSwitch (Switch):

    def __init__ (self,
                  name: str,
                  client_stub: SwitchboardStub):

        Switch.__init__(self, name)
        self.stub = client_stub


    @doc_inherit
    async def set_specification(self,
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

        return (await self.stub.set_specification(req)).value


    @doc_inherit
    async def add_dependency(self,
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

        return (await self.stub.add_dependency(req)).value


    @doc_inherit
    async def remove_dependency(self,
                                predecessor_name: str,
                                reevaluate: bool = True,
                                ) -> bool:

        req = cc.protobuf.RemoveDependencyRequest(
            switch_name = self.name,
            predecessor_name = predecessor_name,
            reevaluate = reevaluate)

        return (await self.stub.remove_dependency(req)).value


    @doc_inherit
    async def add_interceptor(self,
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

        return (await self.stub.add_interceptor(req)).value


    @doc_inherit
    async def remove_interceptor(self,
                                 interceptor_name: str,
                                 ) -> bool:

        req = RemoveInterceptorRequest(
            switch_name = self.name,
            interceptor_name = interceptor_name)

        return (await self.stub.remove_interceptor(req)).value


    @doc_inherit
    async def invoke_interceptor(self,
                                 interceptor_name : str,
                                 state : Optional[int] = None
                                 ) -> Optional[Error]:

        req = InterceptorInvocation(
            switch_name = self.name,
            interceptor_name = interceptor_name,
            state = state)

        result = await self.stub.invoke_intereptor(req)
        return result.error

    @doc_inherit
    async def set_target(self,
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

        return (await self.stub.set_target(req)).updated


    @doc_inherit
    async def set_attributes(self,
                             attributes: Optional[PyValueDict] = None,
                             clear_existing: bool = False):

        req = SetAttributesRequest(
            switch_name = self.name,
            attributes = encodeKeyValueMap(attributes),
            clear_existing = clear_existing)

        return await (self.stub.set_attributes(req)).updated


    @doc_inherit
    async def get_culprits(self,
                           expected_position: bool = True) -> Mapping[str, Status]:

        req = CulpritsQuery(switch_name = self.name,
                            expected = expected_position)

        return (await self.stub.get_culprits(req)).map
