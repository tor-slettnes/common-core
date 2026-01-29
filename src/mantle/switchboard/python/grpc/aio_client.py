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
from cc.protobuf.status import encodeException
from cc.protobuf.variant import PyValueList, encodeValueList
from cc.messaging.grpc import SignalClient, AsyncMixIn

### Switchboard modules
from ..protobuf import (
    AddSwitchRequest, RemoveSwitchRequest, ImportRequest,
    State,
    InterceptorInvocation, InterceptorResult, InterceptorUpdate,
)

from ..base.baseboard import SwitchboardBase
from .aio_remote_switch import AsyncRemoteSwitch

class AsyncClient (AsyncMixIn, SwitchboardBase, SignalClient):
    '''
    Switchboard abstract base
    '''

    from .switchboard_service_pb2_grpc import SwitchboardStub as Stub

    def __init__(self,
                 host: str = "",
                 wait_for_ready: bool = True,
                 watch_all: bool = True,
                 product_name: str|None = None,
                 project_name: str|None = None,
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

        SignalClient.__init__(self,
                              host = host,
                              wait_for_ready = wait_for_ready,
                              watch_all = watch_all,
                              product_name = product_name,
                              project_name = project_name)
        SwitchboardBase.__init__(self)

        self.init_intercept()

    @override
    def _new_switch(self, switch_name: str) -> AsyncRemoteSwitch:
        '''Obtain a new Switch instance in response to update signals from server'''
        return AsyncRemoteSwitch(switch_name, self)


    @override
    async def get_or_add_switch(self,
                                switch_name: str,
                                initial_value: bool|None = None,
                                ) -> AsyncRemoteSwitch:

        switch = self.get_switch(switch_name)
        if switch is None:
            switch = self.switches.setdefault(
                switch_name,
                self._new_switch(switch_name))

            await self.add_switch(switch_name)
            if initial_value is not None:
                await switch.set_active(initial_value)

        return switch


    @override
    async def add_switch(self, switch_name: str) -> bool:
        req = AddSwitchRequest(switch_name = switch_name)
        return (await self.stub.AddSwitch(req)).value

    @override
    async def remove_switch(self, switch_name: str, propagate: bool = True) -> bool:
        req = RemoveSwitchRequest(switch_name = switch_name,
                                  propagate = propagate)
        return (await self.stub.RemoveSwitch(req)).value

    @override
    async def import_switches(self,
                              declarations: PyValueList) -> int:
        req = ImportRequest(
            declarations = encodeValueList(declarations))
        return (await self.stub.ImportSwitches(req)).import_count

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

    async def _intercept_queue_iterator(self):
        while msg := await self.interceptor_update_queue.get():
            yield msg

    async def enqueue_interceptor_update(self, msg: InterceptorUpdate):
        '''
        Enqueue and send an interceptor update to the Switchboard service.
        '''
        self.start_intercepting()
        await self.interceptor_update_queue.put(msg)

    async def _intercept_runner(self):
        runner_tasks = set()
        async for request in self.interceptor_stream:
            task = asyncio.create_task(self.on_interceptor_invocation(request))
            runner_tasks.add(task)
            task.add_done_callback(runner_tasks.discard)

    async def on_interceptor_invocation(self, request: InterceptorInvocation):
        self.logger.info("%s switch %r interceptor %r starting" % (
            self,
            request.switch_name,
            request.interceptor_name,
        ))

        result = InterceptorResult()

        try:
            sw = self.get_switch(request.switch_name, required=True)
            await sw.on_intercept(
                interceptor_name = request.interceptor_name,
                state = State(request.state),
            )

        except Exception as e:
            encodeException(
                e,
                origin = sys.modules['__main__'].__spec__.name,
                output = result.error,
            )

            self.logger.error("%s switch %r interceptor %r failed: [%s] %s" % (
                self,
                request.switch_name,
                request.interceptor_name,
                type(e).__name__,
                e
            ))
        else:
            self.logger.info("%s switch %r interceptor %r completed" % (
                self,
                request.switch_name,
                request.interceptor_name,
            ))

        update = InterceptorUpdate(
            switch_name = request.switch_name,
            interceptor_name = request.interceptor_name,
            invocation_result = result,
        )
        self.logger.warning(
            "Adding to interceptor queue: %s" % (
                update,
            ))

        self.interceptor_update_queue.put_nowait(update)
