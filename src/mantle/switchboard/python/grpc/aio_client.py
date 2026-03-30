'''
Satellite Switchboard implementation that communicates with a Switchboard
service over gRPC.  This flavor implements Python AsyncIO semantics.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from typing import Sequence, Mapping
import asyncio
import sys

### Common Core modules
from cc.core.decorators import override
from cc.core.invocation import method_path, safe_await
from cc.core.paths import FilePathInput
from cc.core.settingsstore import SettingsStore
from cc.protobuf.status import encodeError, Error
from cc.protobuf.variant import PyValueMap, KeyValueMap, decodeKeyValueMap
from cc.messaging.grpc import SignalClient, AsyncMixIn

### Switchboard modules
from ..protobuf import (
    SwitchboardDissecter,
    SwitchInfo, SwitchSelectionInput,
    Specification, Status,
    State, StateSet,
    DependencyStatus, DependencyPolarity,
    InterceptorMethod, InterceptorInvocation, InterceptorResult,
    InterceptorPhase, ExceptionHandling, InvocationStyle, CascadeStyle,
)

from ..base import AsyncSwitch
from .base_client import BaseClient

class AsyncClient (AsyncMixIn, BaseClient):
    '''
    Python AsyncIO client for Switchboard gRPC service.
    '''

    @override
    def _new_switch(self, switch_name: str) -> AsyncSwitch:
        return AsyncSwitch(switch_name, self)

    async def get_or_add_switch(self,
                                switch_name: str,
                                initially_active: bool = False,
                                ) -> AsyncSwitch:

        proxy, added = self._get_or_add_switch_proxy(switch_name, initially_active)

        if added:
            await self.call_add_switch(switch_name, initially_active)

        return proxy

    @override
    async def add_switch(self,
                         switch_name: str,
                         initially_active: bool = False) -> AsyncSwitch:

        proxy, _ = self._get_or_add_switch_proxy(switch_name, initially_active)
        await self.call_add_switch(switch_name, initially_active)
        return proxy


    @override
    @SwitchboardDissecter.decode_response
    async def remove_switch(self,
                            switch_name: str,
                            propagate: bool = True,
                            ) -> bool:
        return await BaseClient.call_remove_switch(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def clear_switches(self,
                             reload: bool = False,
                             ) -> bool:
        return await BaseClient.call_clear_switches(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def import_switches(self,
                              declarations: PyValueMap,
                              replace_specifications: bool = False,
                              replace_statuses: bool = False,
                              invoke_interceptors: InvocationStyle = InvocationStyle.INDIRECT,
                              ) -> int:
        return await BaseClient.call_import_switches(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def export_switches(self,
                              selection: SwitchSelectionInput|None = None,
                              include_specifications: bool = False,
                              include_statuses: bool = True) -> Mapping[str, Mapping]:

        return await BaseClient.call_export_switches(**locals())

    @override
    async def save_switches(self,
                            filename: FilePathInput,
                            selection: SwitchSelectionInput|None = None,
                            include_specifications: bool = False,
                            include_statuses: bool = True):

        declarations = await self.export_switches(
            selection,
            include_specifications,
            include_statuses,
        )

        store = SettingsStore()
        store.update(declarations)
        return store.save(filename)

    @override
    @SwitchboardDissecter.decode_response
    async def get_switch_info(self,
                              selection: SwitchSelectionInput|None = None,
                              with_ancestors: bool = False,
                              ) -> Mapping[str, SwitchInfo]:
        return await BaseClient.call_get_switch_info(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def set_specification(self,
                                switch_name: str,
                                specification: Specification,
                                replace_aliases: bool = False,
                                replace_localizations: bool = False,
                                replace_dependencies: bool = False,
                                replace_interceptors: bool = False,
                                active: bool|None = None,
                                update_state: bool|None = None) -> bool:

        return await BaseClient.call_set_specification(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def get_specifications(self,
                                 selection: SwitchSelectionInput|None = None,
                                 with_ancestors: bool = False,
                                 ) -> Mapping[str, Status]:

        return await BaseClient.call_get_specifications(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def add_dependency(self,
                             switch_name: str,
                             predecessor_name: str,
                             trigger_states: StateSet = State.SETTLED,
                             polarity: DependencyPolarity = DependencyPolarity.POSITIVE,
                             hard: bool = False,
                             sufficient: bool = False,
                             allow_update: bool|None = None,
                             reevaluate: bool|None = None,
                             ) -> bool:

        return await BaseClient.call_add_dependency(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def remove_dependency(self,
                                switch_name: str,
                                predecessor_name: str,
                                reevaluate: bool = True,
                                ) -> bool:

        return await BaseClient.call_remove_dependency(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def get_ancestors(self,
                            switch_name: str) -> Sequence[str]:

        return await BaseClient.call_get_ancestors(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def get_descendants(self,
                              switch_name: str) -> Sequence[str]:

        return await BaseClient.call_get_descendants(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def set_target(self,
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
                         ) -> bool:

        return await BaseClient.call_set_target(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def get_statuses(self,
                           selection: SwitchSelectionInput|None = None,
                           with_ancestors: bool = False,
                           ) -> Mapping[str, Status]:

        return await BaseClient.call_get_statuses(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def get_dependency_status(self,
                                    switch_name: str,
                                    ) -> Mapping[str, DependencyStatus]:

        return await BaseClient.call_get_dependency_status(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def set_attributes(self,
                             switch_name: str,
                             attributes: PyValueMap|None = None,
                             clear_existing: bool = False):

        return await BaseClient.call_set_attributes(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def get_attributes(self,
                             switch_name: str,
                             inherit: bool = False) -> dict:
        return await BaseClient.call_get_attributes(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def get_culprits(self,
                           switch_name: str,
                           expected_position: bool = True) -> Mapping[str, Status]:

        return await BaseClient.call_get_culprits(**locals())

    @override
    @SwitchboardDissecter.decode_response
    async def get_errors(self,
                         switch_name: str,
                         ) -> Mapping[str, Error]:
        return await BaseClient.call_get_errors(**locals())


    @override
    @SwitchboardDissecter.decode_response
    async def invoke_interceptor(self,
                                 interceptor_name: str,
                                 switch_name: str,
                                 state: State|None = None
                                 ) -> Error|None:

        response = await BaseClient.call_invoke_interceptor(**locals())

        if response.Hasfield('error'):
            return response.error
        else:
            return None


    @override
    @SwitchboardDissecter.decode_response
    async def add_interceptor(self,
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
                              future: bool = False) -> bool:

        return await BaseClient.call_add_interceptor(**locals())


    @override
    @SwitchboardDissecter.decode_response
    async def remove_interceptor(self,
                                 interceptor_name: str,
                                 switch_selection: SwitchSelectionInput|None = None,
                                 abandon_pending: bool = True,
                                 ) -> bool:

        return await BaseClient.call_remove_interceptor(**locals())


    def init_intercept(self):
        BaseClient.init_intercept(self)
        self.interceptor_task = None
        self.interceptor_response_queue = None


    def is_intercepting(self):
        if t := self.interceptor_task:
            return not t.done()
        else:
            return False


    def start_intercepting(self):
        if not self.is_intercepting():
            self.interceptor_response_queue = asyncio.Queue()
            self.interceptor_stream = self.stub.Intercept(
                self._intercept_queue_iterator(),
                None)
            self.interceptor_task = asyncio.create_task(
                self._intercept_runner(),
                name = "Intercept Runner")


    def stop_intercepting(self):
        if self.is_intercepting():
            self.interceptor_response_queue.put(None)
            self.interceptor_task.cancel()


    @override
    async def register_decorated_handlers(self, instance: object):
        await self.register_decorated_interceptors(instance)
        self.connect_decorated_handlers(instance)


    @override
    async def register_decorated_interceptors(self, instance: object):
        count = 0
        for (unbound_method, kwargs) in self.decorated_interceptor_map.items():
            method_name = unbound_method.__name__
            if getattr(type(instance), method_name, None) == unbound_method:
                count += 1
                if bound_method := getattr(instance, method_name, None):
                    await self.add_interceptor(
                        interceptor_name = method_path(bound_method),
                        callback = bound_method,
                        **kwargs)

        return count


    async def _intercept_queue_iterator(self):
        while msg := await self.interceptor_response_queue.get():
            yield msg


    async def _intercept_runner(self):
        async with asyncio.TaskGroup() as tg:
            async for request in self.interceptor_stream:
                tg.create_task(
                    self._on_interceptor_invocation(request),
                    name = request.interceptor_name,
                )

    async def _on_interceptor_invocation(self, invocation: InterceptorInvocation):
        if method := self.interceptor_methods.get(invocation.interceptor_name):
            result, error = await safe_await(
                method,
                args = (self.decode(invocation),),
                description = 'switch %r interceptor %r' % (
                    invocation.switch_name,
                    invocation.interceptor_name,
                ),
                log_call = self.logger.debug,
                log_failure = self.logger.error)
        else:
            error = None

        self._enqueue_interceptor_result(
            self.interceptor_response_queue,
            invocation,
            error,
        )

