'''
Switch controlled via a remote gRPC service - AsyncIO flavor.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


from typing import Optional
from typing import Sequence, Mapping
from weakref import ref
import asyncio

from cc.core.decorators import override
from cc.core.invocation import safe_invoke_maybe_async
from cc.protobuf.status import Error, encodeError
from cc.protobuf.dissecter import decode_message
from cc.protobuf.variant import PyValueMap, encodeKeyValueMap

from ..protobuf import (
    Specification, Status, State, StateSet,
    ExceptionHandling, CascadeStyle,
    Dependency, DependencyPolarity,
    InterceptorResult,
)

from ..base import SubscriptionCallback
from .remote_switch_base import RemoteSwitchBase
from .switchboard_service_pb2_grpc import SwitchboardStub

class AsyncRemoteSwitch (RemoteSwitchBase):

    pending_callbacks = set()

    @override
    def publish_update_to(self, callback: SubscriptionCallback):
        safe_invoke_maybe_async(callback, args = (self,))

    @override
    async def set_specification(self,
                                specification: Specification,
                                replace_aliases: bool = False,
                                replace_localizations: bool = False,
                                replace_dependencies: bool = False,
                                replace_interceptors: bool = False,
                                active: Optional[bool] = None,
                                update_state: Optional[bool] = None,
                                ):
        response = await RemoteSwitchBase.set_specification(**locals())
        return response.value


    @override
    async def add_dependency(self,
                             predecessor: str,
                             trigger_states: StateSet = State.SETTLED,
                             polarity: DependencyPolarity = DependencyPolarity.POSITIVE,
                             hard: bool = False,
                             sufficient: bool = False,
                             allow_update: Optional[bool] = None,
                             reevaluate: Optional[bool] = None,
                             ) -> bool:

        response = await RemoteSwitchBase.add_dependency(**locals())
        return response.value


    @override
    async def remove_dependency(self,
                                predecessor: str,
                                reevaluate: bool = True,
                                ) -> bool:

        response = await RemoteSwitchBase.remove_dependency(**locals())
        return response.value



    @override
    async def invoke_interceptor(self,
                                 interceptor_name : str,
                                 state : Optional[int] = None
                                 ) -> Optional[Error]:

        response = await RemoteSwitchBase.invoke_interceptor(**locals())
        return response.error


    @override
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

    @override
    async def on_intercept(self,
                           interceptor_name : str,
                           state : State) -> InterceptorResult:
        response = RemoteSwitchBase.on_intercept(**locals())
        if asyncio.iscoroutine(response):
            await response


    @override
    async def set_target(self,
                         target_state: Optional[State] = None,
                         error: Error|Exception|str|None = None,
                         attributes: Optional[PyValueMap] = None,
                         clear_existing: bool = False,
                         invoke_interceptors: bool = True,
                         cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                         reevaluate: bool = False,
                         on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                         on_error: ExceptionHandling = ExceptionHandling.DEFAULT,
                         ) -> bool:

        response = await RemoteSwitchBase.set_target(**locals())
        return response.updated


    @override
    async def set_attributes(self,
                             attributes: Optional[PyValueMap] = None,
                             clear_existing: bool = False):

        response = await RemoteSwitchBase.set_attributes(**locals())
        return response.updated


    @override
    async def get_culprits(self,
                           expected_position: bool = True) -> Mapping[str, Status]:

        response = await RemoteSwitchBase.get_culprits(**locals())
        return decode_message(response).map


    @override
    async def get_errors(self) -> Mapping[str, Error]:
        response = await RemoteSwitchBase.get_errors(**locals())
        return decode_message(response).map
