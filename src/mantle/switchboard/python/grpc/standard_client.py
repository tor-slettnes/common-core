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
from logging import Logger
import sys

### Common Core modules
from cc.core.decorators import override
from cc.core.invocation import method_path, safe_invoke
from cc.core.paths import FilePathInput
from cc.core.settingsstore import SettingsStore
from cc.protobuf.status import encodeError, Error
from cc.protobuf.variant import PyValueMap, decodeKeyValueMap

### Switchboard modules
from ..protobuf import (
    SwitchboardDissecter,
    SwitchInfo, SwitchSelectionInput, Specification, Status,
    State, StateSet,
    Dependency, DependencyStatus, DependencyPolarity,
    InterceptorMethod, InterceptorInvocation, InterceptorResult,
    InterceptorPhase, ExceptionHandling, InvocationStyle, CascadeStyle,
    Signal,
)

from ..base import Switch, AddSwitchResult
from .base_client import BaseClient

class StandardClient (BaseClient):
    '''
    Python client for Switchboard gRPC service.
    '''

    def __init__(self,
                 host: str = "",
                 wait_for_ready: bool = True,
                 watch_all: bool = True,
                 product_name: str|None = None,
                 project_name: str|None = None,
                 logger: Logger|None = None,
                 ):

        self._switch_lock = Lock()
        BaseClient.__init__(**locals())

    @override
    def _new_switch(self, switch_name: str) -> Switch:
        return Switch(switch_name, self)


    @override
    def _get_or_add_switch_proxy(self,
                                 switch_name: str,
                                 initially_active: bool = False,
                                 ) -> AddSwitchResult:
        with self._switch_lock:
            return BaseClient._get_or_add_switch_proxy(**locals())


    @override
    def _get_or_map_switch(self, msg: Signal) -> Switch|None:
        with self._switch_lock:
            return BaseClient._get_or_map_switch(**locals())


    @override
    def get_or_add_switch(self,
                          switch_name: str,
                          initially_active: bool = False,
                          ) -> AddSwitchResult:

        proxy, added = self._get_or_add_switch_proxy(switch_name, initially_active)

        if added:
            response = self.call_add_switch(switch_name, initially_active)
            added = response.value

        return AddSwitchResult(proxy, added)

    @override
    def add_switch(self,
                   switch_name: str,
                   initially_active: bool = False) -> AddSwitchResult:

        proxy, _ = self._get_or_add_switch_proxy(switch_name, initially_active)
        response = self.call_add_switch(switch_name, initially_active)
        return AddSwitchResult(proxy, response.value)

    @override
    @SwitchboardDissecter.decode_response
    def remove_switch(self,
                      switch_name: str,
                      propagate: bool = True,
                      ) -> bool:

        return BaseClient.call_remove_switch(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def clear_switches(self,
                       reload: bool = False,
                       ) -> bool:

        return BaseClient.call_clear_switches(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def import_switches(self,
                        declarations: PyValueMap,
                        replace_specifications: bool = False,
                        replace_statuses: bool = False,
                        invoke_interceptors: InvocationStyle = InvocationStyle.INDIRECT,
                        ) -> int:

        return BaseClient.call_import_switches(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def export_switches(self,
                        selection: SwitchSelectionInput|None = None,
                        include_specifications: bool = False,
                        include_statuses: bool = True) -> Mapping[str, Mapping]:

        return BaseClient.call_export_switches(**locals())

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
    @SwitchboardDissecter.decode_response
    def get_switch_info(self,
                        selection: SwitchSelectionInput|None = None,
                        with_ancestors: bool = False,
                        ) -> Mapping[str, object]:
        response = BaseClient.call_get_switch_info(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def set_specification(self,
                          switch_name: str,
                          specification: Specification,
                          replace_aliases: bool = False,
                          replace_localizations: bool = False,
                          replace_dependencies: bool = False,
                          replace_interceptors: bool = False,
                          active: bool|None = None,
                          update_state: bool|None = None) -> bool:

        return BaseClient.call_set_specification(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def get_specifications(self,
                           selection: SwitchSelectionInput|None = None,
                           with_ancestors: bool = False,
                           ) -> Mapping[str, object]:

        return BaseClient.call_get_specifications(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def add_dependency(self,
                       switch_name: str,
                       predecessor_name: str,
                       trigger_states: StateSet = State.SETTLED,
                       polarity: DependencyPolarity = DependencyPolarity.POSITIVE,
                       hard: bool = False,
                       sufficient: bool = False,
                       allow_update: bool|None = None,
                       reevaluate: bool|None = None,
                       ) -> bool:

        return BaseClient.call_add_dependency(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def remove_dependency(self,
                          switch_name: str,
                          predecessor_name: str,
                          reevaluate: bool = True,
                          ) -> bool:

        return BaseClient.call_remove_dependency(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def get_dependencies(self,
                         switch_name: str) -> Mapping[str, Dependency]:
        return BaseClient.call_get_dependencies(self, switch_name)


    @override
    @SwitchboardDissecter.decode_response
    def get_ancestors(self,
                      switch_name: str) -> Sequence[str]:

        return BaseClient.call_get_ancestors(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def get_descendants(self,
                        switch_name: str) -> Sequence[str]:

        return BaseClient.call_get_descendants(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def set_target(self,
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

        return BaseClient.call_set_target(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def get_statuses(self,
                     selection: SwitchSelectionInput|None = None,
                     with_ancestors: bool = False,
                     ) -> Mapping[str, object]:

        return BaseClient.call_get_statuses(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def get_dependency_status(self,
                              switch_name: str,
                              ) -> Mapping[str, object]:

        return BaseClient.call_get_dependency_status(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def set_attributes(self,
                       switch_name: str,
                       attributes: PyValueMap|None = None,
                       clear_existing: bool = False):

        return BaseClient.call_set_attributes(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def get_attributes(self,
                       switch_name: str,
                       inherit: bool = False) -> dict:
        return BaseClient.call_get_attributes(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def get_culprits(self,
                     switch_name: str,
                     expected_position: bool = True) -> Mapping[str, object]:

        return BaseClient.call_get_culprits(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def get_errors(self,
                   switch_name: str,
                   ) -> Mapping[str, object]:
        return BaseClient.call_get_errors(**locals())

    @override
    @SwitchboardDissecter.decode_response
    def invoke_interceptor(self,
                           interceptor_name: str,
                           switch_name: str,
                           state: State|None = None
                           ) -> Error|None:

        response = BaseClient.call_invoke_interceptor(**locals())

        if response.Hasfield('error'):
            return response.error
        else:
            return None


    @override
    @SwitchboardDissecter.decode_response
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

        return BaseClient.call_add_interceptor(**locals())


    @override
    @SwitchboardDissecter.decode_response
    def remove_interceptor(self,
                           interceptor_name: str,
                           switch_selection: SwitchSelectionInput|None = None,
                           abandon_pending: bool = True,
                           ) -> bool:

        return BaseClient.call_remove_interceptor(**locals())


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
                args = (self.decode(invocation),),
                description = 'switch %r interceptor %r' % (
                    invocation.switch_name,
                    invocation.interceptor_name,
                ),
                log_call = self.logger.debug,
                log_failure = self.logger.error,
            )
        else:
            error = None

        self._enqueue_interceptor_result(
            self.interceptor_response_queue,
            invocation,
            error,
        )

