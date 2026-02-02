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

### Switchboard modules
from ..protobuf import (
    State,
    InterceptorInvocation, InterceptorResult, InterceptorUpdate,
)

from .base_client import BaseClient
from .remote_switch import RemoteSwitch


class Client (BaseClient):
    '''
    Python client for Switchboard gRPC service.
    '''

    @override
    def _new_switch(self, switch_name: str) -> RemoteSwitch:
        '''Obtain a new Switch instance in response to update signals from server'''
        return RemoteSwitch(switch_name, self)

    @override
    def add_switch(self,
                   switch_name: str,
                   active: bool = False) -> bool:
        response = BaseClient.add_switch(**locals())
        return response.value

    @override
    def remove_switch(self,
                      switch_name: str,
                      propagate: bool = True,
                      ) -> bool:
        response = BaseClient.remove_switch(**locals())
        return response.value

    @override
    def import_switches(self,
                        declarations: PyValueList) -> int:
        response = BaseClient.import_switches(**locals())
        return response.import_count

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
        with self.interceptor_lock:
            if t := self.interceptor_thread:
                self.interceptor_thread = None
                self.interceptor_update_queue.put(None)
                self.interceptor_stream.throw(StopIteration)
                if wait and t.is_alive():
                    t.join()

    def enqueue_interceptor_update(self, msg: InterceptorUpdate):
        self.start_intercepting()
        self.interceptor_update_queue.put_nowait(msg)


    def _intercept_runner(self):
        for request in self.interceptor_stream:
            t = Thread(
                target = self._on_interceptor_invocation,
                args = (request,),
                daemon = True)
            t.start()

    def _on_interceptor_invocation(self, request: InterceptorInvocation):
        self.logger.debug("%s switch %r interceptor %r starting" % (
            self,
            request.switch_name,
            request.interceptor_name,
        ))

        error = None
        try:
            sw = self.get_switch(request.switch_name, required=True)
            sw.on_intercept(
                interceptor_name = request.interceptor_name,
                state = State(request.state),
            )
        except Exception as e:
            error = e

        self._return_interceptor_response(request, error)
