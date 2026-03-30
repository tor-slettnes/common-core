'''
Abstract representation of a Switchboard node ("Switch")
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard ProtoBuf modules
from typing import Mapping, Sequence, Set, Callable, Optional
from weakref import ref

### Core modules
from cc.core.decorators import override
from cc.core.invocation import safe_invoke_maybe_async
from cc.protobuf.status import Error, encodeError, Level
from cc.protobuf.wellknown import BoolValue
from cc.protobuf.variant import (
    PyValueMap, KeyValueMap,
    encodeKeyValueMap, decodeKeyValueMap,
)

### Swithboard modules
from ..protobuf import (
    Specification, LanguageCode,
    Status, StatusMap, ErrorMap,
    State, StateSet,
    DependencyStatus, DependencyPolarity,
    Localization, LocalizationsInput,
    InterceptorMethod, InterceptorPhase, ExceptionHandling,
    InvocationStyle, CascadeStyle,
    GetAttributesResponse,
    DEFAULT_LANGUAGE,
)

from .switch import (
    Switch,
    SwitchUpdateSubscriber,
)

class AsyncSwitch (Switch):
    '''
    Local proxy object representing a single switch with AsyncIO calling
    semantics.
    '''

    @override
    def publish_update_to(self, callback: SwitchUpdateSubscriber):
        safe_invoke_maybe_async(
            callback,
            args = (self,),
            log_call = self.logger.debug,
            log_failure = self.logger.error,
        )

    @override
    async def set_specification(self,
                                specification: Specification,
                                replace_aliases: bool = False,
                                replace_localizations: bool = False,
                                replace_dependencies: bool = False,
                                replace_interceptors: bool = False,
                                active: bool|None = None,
                                update_state: bool|None = None) -> bool:

        return await Switch.set_specification(**locals())

    @override
    async def get_specification(self):
        specs = await self.board.get_specifications(selection = [self.name])
        return specs.get(self.name)

    @override
    async def get_status(self) -> Status:
        statues = await self.board.get_specifications(selection = [self.name])
        return statuses.get(self.name)

    @override
    async def set_localizations(self,
                          localizations: LocalizationsInput,
                          replace: bool = False,
                          ) -> bool:
        return await Switch.set_localizations(**locals())

    @override
    async def set_localization(self,
                               language_code: LanguageCode = DEFAULT_LANGUAGE,
                               localization: Localization|None = None,
                               *,
                               description: str|None = None,
                               activate_text: str|None = None,
                               deactivate_text: str|None = None,
                               state_texts: Mapping[State, str]|None  = None
                               ) -> bool:
        return await Switch.set_localization(**locals())


    @override
    async def add_dependency(self,
                             predecessor: str,
                             trigger_states: StateSet = State.SETTLED,
                             polarity: DependencyPolarity = DependencyPolarity.POSITIVE,
                             hard: bool = False,
                             sufficient: bool = False,
                             allow_update: Optional[bool] = None,
                             reevaluate: Optional[bool] = None,
                             ) -> bool:

        return await Switch.add_dependency(**locals())


    @override
    async def remove_dependency(self,
                                predecessor: str,
                                reevaluate: bool = True,
                                ) -> bool:

        return await Switch.remove_dependency(**locals())


    @override
    async def invoke_interceptor(self,
                                 interceptor_name : str,
                                 state : Optional[int] = None
                                 ) -> Optional[Error]:

        return await Switch.invoke_interceptor(**locals())

    @override
    async def add_interceptor(self,
                              interceptor_name: str,
                              state_transitions: StateSet,
                              callback: InterceptorMethod,
                              phase: InterceptorPhase = InterceptorPhase.NORMAL,
                              asynchronous: bool = False,
                              rerun: bool = False,
                              immediate: bool = False,
                              future: bool = False,
                              on_cancel: ExceptionHandling = ExceptionHandling.ABORT,
                              on_error: ExceptionHandling = ExceptionHandling.FAIL,
                              ) -> bool:
        return await Switch.add_interceptor(**locals())

    @override
    async def remove_interceptor(self,
                                 interceptor_name: str,
                                 abandon_pending: bool = True,
                                 ) -> bool:
        return await Switch.remove_interceptor(**locals())

    @override
    async def set_target(self,
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

        return await Switch.set_target(**locals())

    @override
    async def set_active(self,
                         active: bool,
                         attributes: PyValueMap|None = None,
                         clear_existing: bool = False,
                         invoke_interceptors: InvocationStyle = InvocationStyle.ALL,
                         cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                         reenter: bool = False,
                         on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                         on_error: ExceptionHandling = ExceptionHandling.DEFAULT):
        return await Switch.set_active(**locals())

    @override
    async def set_error(self,
                        error: Error|Exception|str,
                        attributes: PyValueMap|None = None,
                        clear_existing: bool = False,
                        invoke_interceptors: InvocationStyle = InvocationStyle.ALL,
                        cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                        reenter: bool = True,
                        on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                        on_error: ExceptionHandling = ExceptionHandling.DEFAULT):
        return await Switch.set_error(**locals())

    @override
    async def set_auto(self,
                       attributes: PyValueMap|None = None,
                       clear_existing: bool = False,
                       invoke_interceptors: InvocationStyle = InvocationStyle.ALL,
                       cascade_descendants: CascadeStyle = CascadeStyle.ASYNC,
                       reenter: bool = False,
                       on_cancel: ExceptionHandling = ExceptionHandling.DEFAULT,
                       on_error: ExceptionHandling = ExceptionHandling.DEFAULT):
        return await Switch.set_auto(**locals())

    @override
    async def get_status(self) -> Status:
        return await Switch.get_status(**locals())

    @override
    async def get_dependency_statuses(self) -> Mapping[str, DependencyStatus]:
        return await Switch.get_dependency_statuses(**locals())

    @override
    async def get_attributes(self,
                             inherit: bool = False) -> GetAttributesResponse:
        return await Switch.get_attributes(**locals())

    @override
    async def set_attributes(self,
                             attributes: Optional[PyValueMap] = None,
                             clear_existing: bool = False):
        return await Switch.set_attributes(**locals())

    @override
    async def get_culprits(self,
                           expected_position: bool = True) -> StatusMap:

        return await Switch.get_culprits(**locals())

    @override
    async def get_errors(self) -> ErrorMap:
        return await Switch.get_errors(**locals())
