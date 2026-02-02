'''
Satellite Switchboard implementation that communicates with a Switchboard
service over gRPC.  This flavor implements Python AsyncIO semantics.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
import asyncio
import sys

### Common Core modules
from cc.core.decorators import override
from cc.core.invocation import invoke_async
from cc.core.paths import FilePathInput
from cc.protobuf.status import encodeError
from cc.protobuf.variant import PyValueList, encodeValueList
from cc.messaging.grpc import SignalClient, AsyncMixIn

### Switchboard modules
from ..protobuf import (
    State,
    InterceptorInvocation, InterceptorResult, InterceptorUpdate,
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
                         active: bool = False) -> bool:
        response = await BaseClient.add_switch(**locals())
        return response.value

    @override
    async def remove_switch(self, switch_name: str, propagate: bool = True) -> bool:
        response = await BaseClient.remove_switch(**locals())
        return response.value

    @override
    async def import_switches(self,
                              declarations: PyValueList) -> int:
        response = await BaseClient.import_switches(**locals())
        return response.import_count

    @override
    async def load_switches(self,
                            filename: FilePathInput):
        await BaseClient.load_switches(**locals())


    def init_intercept(self):
        self.interceptor_task = None
        self.interceptor_update_queue = None

    def is_intercepting(self):
        if t := self.interceptor_task:
            return not t.done()
        else:
            return False

    def start_intercepting(self):
        if not self.is_intercepting():
            self.interceptor_update_queue = asyncio.Queue()
            self.interceptor_stream = self.stub.Intercept(
                self._intercept_queue_iterator(),
                None)
            self.interceptor_task = asyncio.create_task(
                self._intercept_runner())

    def stop_intercepting(self):
        if self.is_intercepting():
            self.interceptor_update_queue.put(None)
            self.interceptor_task.cancel()

    def enqueue_interceptor_update(self, msg: InterceptorUpdate):
        self.start_intercepting()
        self.interceptor_update_queue.put_nowait(msg)

    async def _intercept_queue_iterator(self):
        while msg := await self.interceptor_update_queue.get():
            yield msg

    async def _intercept_runner(self):
        with asyncio.TaskGroup() as tg:
            async for request in self.interceptor_stream:
                tg.create_task(self._on_interceptor_invocation(request))

    async def _on_interceptor_invocation(self, request: InterceptorInvocation):
        self.logger.info("%s switch %r interceptor %r starting" % (
            self,
            request.switch_name,
            request.interceptor_name,
        ))

        error = None
        try:
            sw = self.get_switch(request.switch_name, required=True)
            await sw.on_intercept(
                interceptor_name = request.interceptor_name,
                state = State(request.state),
            )

        except Exception as e:
            error = e

        self._return_interceptor_response(request, error)
