'''
Satellite Switchboard implementation that communicates with a Switchboard
service over gRPC
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from threading import Thread, Lock
from queue import Queue
import sys

### Common Core modules
from cc.core.decorators import override, virtual
from cc.core.invocation import method_path
from cc.protobuf.status import encodeError
from cc.protobuf.variant import PyValueList, encodeValueList
from cc.messaging.grpc import SignalClient

### Switchboard modules
from ..protobuf import (
    AddSwitchRequest, RemoveSwitchRequest, ImportRequest,
    State,
    InterceptorInvocation, InterceptorResult, InterceptorUpdate,
)
from ..base.baseboard import SwitchboardBase
from .remote_switch import RemoteSwitch


class Client (SwitchboardBase, SignalClient):
    '''
    Switchboard abstract base
    '''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `cc.messaging.grpc.GenericClient` base to instantiate `self.stub`.
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

        SwitchboardBase.__init__(self)
        SignalClient.__init__(
            self,
            host = host,
            wait_for_ready = wait_for_ready,
            watch_all = watch_all,
            product_name = product_name,
            project_name = project_name,
        )

        self.init_intercept()

    def __del__(self):
        self.stop_intercepting()

    @override
    def _new_switch(self, switch_name: str) -> RemoteSwitch:
        '''Obtain a new Switch instance in response to update signals from server'''
        return RemoteSwitch(switch_name, self)

    @override
    def get_or_add_switch(self,
                          switch_name: str,
                          initial_value: bool|None = None,
                          ) -> RemoteSwitch:

        with self._switch_lock:
            switch = self.get_switch(switch_name)

            if switch is None:
                switch = self.switches.setdefault(
                    switch_name,
                    self._new_switch(switch_name))

                self.add_switch(switch_name)
                if initial_value is not None:
                    switch.set_active(initial_value)

            return switch

    @override
    def add_switch(self, switch_name: str) -> bool:
        req = AddSwitchRequest(switch_name = switch_name)
        return self.stub.AddSwitch(req).value

    @override
    def remove_switch(self,
                      switch_name: str,
                      propagate: bool = True,
                      ) -> bool:
        req = RemoveSwitchRequest(switch_name = switch_name,
                                  propagate = propagate)
        return self.stub.RemoveSwitch(req).value

    @override
    def import_switches(self,
                        declarations: PyValueList) -> int:
        req = ImportRequest(
            declarations = encodeValueList(declarations))
        return self.stub.ImportSwitches(req).import_count

    def init_intercept(self):
        self.interceptor_thread = None
        self.interceptor_lock = Lock()
        self.interceptor_update_queue = None

    def is_intercepting(self):
        if t := self.interceptor_thread:
            return t.is_alive()
        else:
            return False

    def start_intercepting(self):
        '''
        Start a worker task to handle Interceptor invocations from the
        server. Does nothing if the task is already running.
        '''

        with self.interceptor_lock:
            if not self.is_intercepting():
                self.interceptor_update_queue = Queue()
                self.interceptor_stream = self.stub.Intercept(
                    iter(self.interceptor_update_queue.get, None))
                self.interceptor_thread = Thread(
                    target = self._intercept_runner,
                    daemon = True)
                self.interceptor_thread.start()

    def stop_intercepting(self, wait=True):
        '''
        Stop any running worker task to handle Interceptor invocations from
        the server.

        @param wait
            Wait for the worker task to finish before returning.
        '''
        with self.interceptor_lock:
            if t := self.interceptor_thread:
                self.interceptor_thread = None
                self.interceptor_update_queue.put(None)
                self.interceptor_stream.throw(StopIteration)
                if wait and t.is_alive():
                    t.join()

    def enqueue_interceptor_update(self, msg: InterceptorUpdate):
        '''
        Enqueue and send an interceptor update to the Switchboard service.
        '''
        self.start_intercepting()
        self.interceptor_update_queue.put_nowait(msg)

    def _intercept_runner(self):
        for request in self.interceptor_stream:
            t = Thread(
                target = self.on_interceptor_invocation,
                args = (request,),
                daemon = True)
            t.start()

    def on_interceptor_invocation(self, request: InterceptorInvocation):
        self.logger.debug("%s switch %r interceptor %r starting" % (
            self,
            request.switch_name,
            request.interceptor_name,
        ))

        result = InterceptorResult()

        try:
            sw = self.get_switch(request.switch_name, required=True)
            sw.on_intercept(
                interceptor_name = request.interceptor_name,
                state = State(request.state),
            )
        except Exception as e:
            self.logger.error("%s switch %r interceptor %r failed: [%s] %s" % (
                self,
                request.switch_name,
                request.interceptor_name,
                type(e).__name__,
                e
            ))
            encodeError(
                e,
                origin = sys.modules['__main__'].__spec__.name,
                output = result.error,
            )
        else:
            self.logger.debug("%s switch %r interceptor %r completed" % (
                self,
                request.switch_name,
                request.interceptor_name,
            ))

        update = InterceptorUpdate(
            switch_name = request.switch_name,
            interceptor_name = request.interceptor_name,
            invocation_result = result,
        )
        self.interceptor_update_queue.put(update)
