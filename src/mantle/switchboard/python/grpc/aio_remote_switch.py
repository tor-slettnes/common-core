'''
Switch controlled via a remote gRPC service - AsyncIO flavor.
'''

__all__ = ['RemoteSwitch']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


from typing import Optional
from collections.abc import Sequence, Mapping
from weakref import ref
import asyncio

from cc.core.decorators import doc_inherit
from cc.protobuf.status import Error
from cc.protobuf.variant import PyValueDict, encodeKeyValueMap

from ..protobuf import (
    Status, State, StateSet, encodeStateSet,
    InterceptorPhase, ExceptionHandling,
    Specification, SetSpecificationRequest,
    Localization, LocalizationMap, Dependency, DependencyMap,
    AddDependencyRequest, RemoveDependencyRequest, DependencyPolarity,
    InterceptorUpdate, InterceptorRegistration, InterceptorDeregistration,
    InterceptorSpec, InterceptorInvocation, InterceptorResult,
    SetTargetRequest, SetAttributesRequest, CulpritsQuery,
)

from ..base.switch import Switch, InterceptorMethod
from .switchboard_service_pb2_grpc import SwitchboardStub

class AsyncRemoteSwitch (Switch):

    def __init__ (self,
                  name: str,
                  client: 'AsyncClient'):

        Switch.__init__(self, name = name)
        self.stub = client.stub
        self.client = ref(client)

    @doc_inherit
    async def set_specification(self,
                                specification: Specification,
                                replace_aliases: bool = False,
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
            replace_aliases = replace_aliases,
            replace_localizations = replace_localizations,
            replace_dependencies = replace_dependencies,
            replace_interceptors = replace_interceptors,
            update_state = update_state)

        return (await self.stub.SetSpecification(req)).value


    @doc_inherit
    async def add_dependency(self,
                             predecessor_name: str,
                             trigger_states: StateSet = State.SETTLED,
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
                trigger_states = encodeStateSet(trigger_states),
                polarity = polarity,
                hard = hard,
                sufficient = sufficient),
            allow_update = allow_update,
            reevaluate = reevaluate)

        return (await self.stub.AddDependency(req)).value


    @doc_inherit
    async def remove_dependency(self,
                                predecessor_name: str,
                                reevaluate: bool = True,
                                ) -> bool:

        req = RemoveDependencyRequest(
            switch_name = self.name,
            predecessor_name = predecessor_name,
            reevaluate = reevaluate)

        return (await self.stub.RemoveDependency(req)).value


    @doc_inherit
    async def add_interceptor(self,
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

            await client.enqueue_interceptor_update(update)

        return is_new


    @doc_inherit
    async def remove_interceptor(self,
                           interceptor_name: str,
                           ) -> bool:

        if client := self.client():
            update = InterceptorUpdate(
                switch_name = self.name,
                interceptor_name = interceptor_name,
                deregistration = InterceptorDeregistration(),
            )
            await client.enqueue_interceptor_update(update)

        return Switch.remove_interceptor(self, interceptor_name)


    @doc_inherit
    async def invoke_interceptor(self,
                                 interceptor_name : str,
                                 state : Optional[int] = None
                                 ) -> Optional[Error]:

        req = InterceptorInvocation(
            switch_name = self.name,
            interceptor_name = interceptor_name,
            state = state)

        result = await self.stub.InvokeInterceptor(req)
        return result.error

    @doc_inherit
    async def on_intercept(self,
                           interceptor_name : str,
                           state : State):
        interceptor = self.interceptor_methods[interceptor_name]
        response = interceptor(self, interceptor_name, state)
        if asyncio.iscoroutine(response):
            await response

    @doc_inherit
    async def set_target(self,
                         target_state: Optional[State] = None,
                         error: Optional[Error] = None,
                         attributes: Optional[PyValueDict] = None,
                         clear_existing: bool = False,
                         with_interceptors: bool = True,
                         trigger_descendants: bool = True,
                         reevaluate: bool = False,
                         on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                         on_error: ExceptionHandling = ExceptionHandling.DEFAULT,
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

        return (await self.stub.SetTarget(req)).updated


    @doc_inherit
    async def set_attributes(self,
                             attributes: Optional[PyValueDict] = None,
                             clear_existing: bool = False):

        req = SetAttributesRequest(
            switch_name = self.name,
            attributes = encodeKeyValueMap(attributes),
            clear_existing = clear_existing)

        return await (self.stub.SetAttributes(req)).updated


    @doc_inherit
    async def get_culprits(self,
                           expected_position: bool = True) -> Mapping[str, Status]:

        req = CulpritsQuery(switch_name = self.name,
                            expected = expected_position)

        return (await self.stub.GetCulprits(req)).map


