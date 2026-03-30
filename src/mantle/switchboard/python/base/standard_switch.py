'''
Switch controlled via a gRPC service
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


### Standard Python modules
from typing import Mapping, Optional

### Common Core modules
from cc.core.decorators import override
from cc.protobuf.status import Error
from cc.protobuf.variant import PyValueMap, KeyValueMap

### Switchboard modules
from ..protobuf import (
    Specification, Status, State, StateSet,
    ExceptionHandling, InvocationStyle, CascadeStyle,
    Dependency, DependencyPolarity, DependencyStatus,
)

from .base_switch_proxy import BaseSwitchProxy
from .switchboard_service_pb2_grpc import SwitchboardStub


class StandardSwitchProxy (BaseSwitchProxy):

    @override
    def set_specification(self,
                          specification: Specification,
                          replace_aliases: bool = False,
                          replace_localizations: bool = False,
                          replace_dependencies: bool = False,
                          replace_interceptors: bool = False,
                          update_state: Optional[bool] = None,
                          ):
        response = BaseSwitchProxy.set_specification(**locals())
        return response.value


    @override
    def get_specification(self):
        response = BaseSwitchProxy.get_specification(**locals())
        return self.to_dataclass(response).get(self.name)

    @override
    def add_dependency(self,
                       predecessor: str,
                       trigger_states: StateSet = State.SETTLED,
                       polarity: DependencyPolarity = DependencyPolarity.POSITIVE,
                       hard: bool = False,
                       sufficient: bool = False,
                       allow_update: Optional[bool] = None,
                       reevaluate: Optional[bool] = None,
                       ) -> bool:

        response = BaseSwitchProxy.add_dependency(**locals())
        return response.value


    @override
    def remove_dependency(self,
                          predecessor: str,
                          reevaluate: bool = True,
                          ) -> bool:

        response = BaseSwitchProxy.remove_dependency(**locals())
        return response.value

    @override
    def invoke_interceptor(self,
                           interceptor_name : str,
                           state : Optional[int] = None
                           ) -> Optional[Error]:

        response = BaseSwitchProxy.invoke_interceptor(**locals())
        if response.Hasfield('error'):
            return self.to_dataclass(response.error)
        else:
            return None

    @override
    def set_target(self,
                   target_state: Optional[State] = None,
                   error: Error|Exception|str|None = None,
                   attributes: Optional[PyValueMap] = None,
                   clear_existing: bool = False,
                   invoke_interceptors: InvocationStyle = InvocationStyle.ALL,
                   cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                   reenter: bool = False,
                   on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                   on_error: ExceptionHandling = ExceptionHandling.DEFAULT,
                   ) -> bool:

        response = BaseSwitchProxy.set_target(**locals())
        return response.updated

    @override
    def get_status(self) -> Status:
        response = BaseSwitchProxy.get_status(**locals())
        return self.to_dataclass(response).get(self.name)

    @override
    def get_dependency_statuses(self) -> Mapping[str, DependencyStatus]:
        response = BaseSwitchProxy.get_dependency_statuses(**locals())
        return self.to_dataclass(response)

    @override
    def get_attributes(self,
                       inherit: bool = False) -> KeyValueMap:
        response = BaseSwitchProxy.get_attributes(**locals())
        return self.to_dataclass(response.attributes)

    @override
    def set_attributes(self,
                       attributes: Optional[PyValueMap] = None,
                       clear_existing: bool = False):

        response = BaseSwitchProxy.set_attributes(**locals())
        return response.updated

    @override
    def get_culprits(self,
                     expected_position: bool = True) -> Mapping[str, Status]:

        response = BaseSwitchProxy.get_culprits(**locals())
        return self.to_dataclass(response)


    @override
    def get_errors(self) -> Mapping[str, Error]:
        response = BaseSwitchProxy.get_errors(**locals())
        return self.to_dataclass(response)
