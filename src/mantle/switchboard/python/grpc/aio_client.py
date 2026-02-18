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
from cc.core.invocation import method_path
from cc.core.paths import FilePathInput
from cc.core.settingsstore import SettingsStore
from cc.protobuf.status import encodeError
from cc.protobuf.variant import PyValueMap, decodeKeyValueMap
from cc.messaging.grpc import SignalClient, AsyncMixIn

### Switchboard modules

from ..protobuf import (
    State, StateSet,
    InterceptorMethod, InterceptorInvocation, InterceptorResult,
    InterceptorPhase, ExceptionHandling,
    SwitchSelectionInput,
)

from .base_client import BaseClient
from .aio_remote_switch import AsyncRemoteSwitch

class AsyncClient (AsyncMixIn, BaseClient):
    '''
    Python AsyncIO client for Switchboard gRPC service.
    '''

    @override
    def _new_switch(self, switch_name: str) -> AsyncRemoteSwitch:
        '''Obtain a new Switch instance in response to update signals from server'''
        return AsyncRemoteSwitch(switch_name, self)

    @override
    async def add_switch(self,
                         switch_name: str,
                         initially_active: bool = False) -> bool:

        switch =  BaseClient.add_switch(**locals())
        await self._call_add_switch(switch_name, initially_active)
        return switch

    @override
    async def remove_switch(self, switch_name: str, propagate: bool = True) -> bool:
        response = await BaseClient.remove_switch(**locals())
        return response.value

    @override
    async def clear_switches(self,
                             reload: bool = False,
                             ) -> bool:
        response = await BaseClient.clear_switches(**locals())
        return response.value

    @override
    async def import_switches(self,
                              declarations: PyValueMap,
                              replace_specifications: bool = False,
                              replace_statuses: bool = True) -> int:
        response = await BaseClient.import_switches(**locals())
        return response.import_count

    @override
    async def export_switches(self,
                              selection: SwitchSelectionInput|None = None,
                              include_specifications: bool = False,
                              include_statuses: bool = True) -> Sequence[Mapping]:
        response = await BaseClient.export_switches(**locals())
        return decodeKeyValueMap(response.declarations)

    @override
    async def load_switches(self,
                            filename: FilePathInput,
                            replace_specifications: bool = False,
                            replace_statuses: bool = True):
        await BaseClient.load_switches(**locals())

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

        response = await BaseClient.add_interceptor(**locals())
        return response.value


    @override
    async def remove_interceptor(self,
                                 interceptor_name: str,
                                 switch_selection: SwitchSelectionInput|None = None,
                                 abandon_pending: bool = True,
                                 ) -> bool:

        response = await BaseClient.remove_interceptor(**locals())
        return response.value


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
                self._intercept_runner())


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
                bound_method = getattr(instance, method_name)
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
                tg.create_task(self._on_interceptor_invocation(request))


    async def _on_interceptor_invocation(self, invocation: InterceptorInvocation):
        self.logger.debug("%s switch %r interceptor %r starting" % (
            self,
            invocation.switch_name,
            invocation.interceptor_name,
        ))

        error = None
        if method := self.interceptor_methods.get(invocation.interceptor_name):
            try:
                result = method(invocation)
                if asyncio.iscoroutine(result):
                    await result
            except Exception as e:
                error = e

        self._enqueue_interceptor_result(
            self.interceptor_response_queue,
            invocation,
            error,
        )

