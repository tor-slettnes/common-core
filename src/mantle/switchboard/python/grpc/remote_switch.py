'''
Switch controlled via a remote gRPC service
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


### Standard Python modules
from typing import Mapping, Optional

### Common Core modules
from cc.core.decorators import override
from cc.protobuf.status import Error
from cc.protobuf.dissecter import decode_message
from cc.protobuf.variant import PyValueMap

### Switchboard modules
from ..protobuf import (
    Specification, Status, State, StateSet,
    ExceptionHandling, CascadeStyle,
    Dependency, DependencyPolarity,
)

from .remote_switch_base import RemoteSwitchBase
from .switchboard_service_pb2_grpc import SwitchboardStub


class RemoteSwitch (RemoteSwitchBase):

    @override
    def set_specification(self,
                          specification: Specification,
                          replace_aliases: bool = False,
                          replace_localizations: bool = False,
                          replace_dependencies: bool = False,
                          replace_interceptors: bool = False,
                          active: Optional[bool] = None,
                          update_state: Optional[bool] = None,
                          ):
        response = RemoteSwitchBase.set_specification(**locals())
        return response.value


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

        response = RemoteSwitchBase.add_dependency(**locals())
        return response.value


    @override
    def remove_dependency(self,
                          predecessor: str,
                          reevaluate: bool = True,
                          ) -> bool:

        response = RemoteSwitchBase.remove_dependency(**locals())
        return response.value


    @override
    def invoke_interceptor(self,
                           interceptor_name : str,
                           state : Optional[int] = None
                           ) -> Optional[Error]:

        response = RemoteSwitchBase.invoke_interceptor(**locals())
        return response.error


    @override
    def set_target(self,
                   target_state: Optional[State] = None,
                   error: Error|Exception|str|None = None,
                   attributes: Optional[PyValueMap] = None,
                   clear_existing: bool = False,
                   invoke_interceptors: bool = True,
                   cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                   reenter: bool = False,
                   on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                   on_error: ExceptionHandling = ExceptionHandling.DEFAULT,
                   ) -> bool:

        response = RemoteSwitchBase.set_target(**locals())
        return response.updated


    @override
    def set_attributes(self,
                       attributes: Optional[PyValueMap] = None,
                       clear_existing: bool = False):

        response = RemoteSwitchBase.set_attributes(**locals())
        return response.updated


    @override
    def get_culprits(self,
                     expected_position: bool = True) -> Mapping[str, Status]:

        response = RemoteSwitchBase.get_culprits(**locals())
        return decode_message(response).map


    @override
    def get_errors(self) -> Mapping[str, Error]:
        response = RemoteSwitchBase.get_errors(**locals())
        return decode_message(response).map
