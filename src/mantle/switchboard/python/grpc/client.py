'''
Satellite Switchboard implementation that communicates with a Switchboard
service over gRPC
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from typing import Sequence, Mapping
from threading import Thread, Lock
from queue import Queue
import sys

### Common Core modules
from cc.core.decorators import override
from cc.core.invocation import method_path, safe_invoke
from cc.core.paths import FilePathInput
from cc.core.settingsstore import SettingsStore
from cc.protobuf.status import encodeError
from cc.protobuf.variant import PyValueMap, decodeKeyValueMap

### Switchboard modules
from ..protobuf import (
    Status, State, StateSet,
    InterceptorMethod, InterceptorInvocation, InterceptorResult,
    InterceptorPhase, ExceptionHandling, InvocationStyle,
    SwitchSelectionInput,
)

from .base_client import BaseClient
from .remote_switch import RemoteSwitch


class Client (BaseClient):
    '''
    Python client for Switchboard gRPC service.
    '''

    @override
    def _new_switch(self, switch_name: str) -> RemoteSwitch:
        '''Obtain a new Switch instance in response to signals from server'''
        return RemoteSwitch(switch_name, self)

    @override
    def get_status(self,
                   selection: SwitchSelectionInput|None = None,
                   with_ancestors: bool = False,
                   ) -> Mapping[str, Status]:
        response = BaseClient.get_status(**locals())
        return response.map

    @override
    def add_switch(self,
                   switch_name: str,
                   initially_active: bool = False) -> RemoteSwitch:

        switch =  BaseClient.add_switch(**locals())
        self._call_add_switch(switch_name, initially_active)
        return switch

    @override
    def remove_switch(self,
                      switch_name: str,
                      propagate: bool = True,
                      ) -> bool:
        response = BaseClient.remove_switch(**locals())
        return response.value

    @override
    def clear_switches(self,
                       reload: bool = False,
                       ) -> bool:
        response = BaseClient.clear_switches(**locals())
        return response.value

    @override
    def import_switches(self,
                        declarations: PyValueMap,
                        replace_specifications: bool = False,
                        replace_statuses: bool = True,
                        invoke_interceptors: InvocationStyle = InvocationStyle.INDIRECT,
                        ) -> int:
        response = BaseClient.import_switches(**locals())
        return response.import_count

    @override
    def export_switches(self,
                        selection: SwitchSelectionInput|None = None,
                        include_specifications: bool = False,
                        include_statuses: bool = True) -> Mapping[str, Mapping]:
        response = BaseClient.export_switches(**locals())
        return decodeKeyValueMap(response.declarations)

    @override
    def save_switches(self,
                      filename: FilePathInput,
                      selection: SwitchSelectionInput|None = None,
                      include_specifications: bool = False,
                      include_statuses: bool = True):

        declarations = self.export_switches(
            selection,
            include_specifications,
            include_statuses,
        )

        store = SettingsStore()
        store.update(declarations)
        return store.save(filename)

    @override
    def add_interceptor(self,
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

        response = BaseClient.add_interceptor(**locals())
        return response.value


    @override
    def remove_interceptor(self,
                           interceptor_name: str,
                           switch_selection: SwitchSelectionInput|None = None,
                           abandon_pending: bool = True,
                           ) -> bool:

        response = BaseClient.remove_interceptor(**locals())
        return response.value

    def init_intercept(self):
        BaseClient.init_intercept(self)
        self.interceptor_thread = None
        self.interceptor_lock = Lock()
        self.interceptor_update_queue = None

    def is_intercepting(self):
        if t := self.interceptor_thread:
            return t.is_alive()
        else:
            return False


    @override
    def start_intercepting(self):
        with self.interceptor_lock:
            if not self.is_intercepting():
                self.interceptor_response_queue = Queue()
                self.interceptor_stream = self.stub.Intercept(
                    iter(self.interceptor_response_queue.get, None))
                self.interceptor_thread = Thread(
                    target = self._intercept_runner,
                    daemon = True)
                self.interceptor_thread.start()


    @override
    def stop_intercepting(self, wait=True):
        with self.interceptor_lock:
            if t := self.interceptor_thread:
                self.interceptor_thread = None
                self.interceptor_response_queue.put(None)
                self.interceptor_stream.throw(StopIteration)
                if wait and t.is_alive():
                    t.join()


    @override
    def register_decorated_interceptors(self, instance: object) -> int:
        count = 0
        for (unbound_method, kwargs) in self.decorated_interceptor_map.items():
            method_name = unbound_method.__name__
            if getattr(type(instance), method_name, None) == unbound_method:
                count += 1
                if bound_method := getattr(instance, method_name, None):
                    self.add_interceptor(
                        interceptor_name = method_path(bound_method),
                        callback = bound_method,
                        **kwargs)

        return count


    def _intercept_runner(self):
        for request in self.interceptor_stream:
            t = Thread(
                target = self._on_interceptor_invocation,
                args = (request,),
                daemon = True)
            t.start()


    def _on_interceptor_invocation(self, invocation: InterceptorInvocation):
        if method := self.interceptor_methods.get(invocation.interceptor_name):
            result, error = safe_invoke(
                method,
                args = (invocation,),
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

