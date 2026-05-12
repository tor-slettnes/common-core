from . import base
from . import protobuf
from . import grpc

from .protobuf import (
    ### Types generated from `switchboard_types.proto`
    Dependency,
    InterceptorInvocation,
    Signal,
    Specification,
    Status,
    SwitchSelection,

    ### Overridden type enumerations
    InvocationStyle,
    CascadeStyle,
    DependencyPolarity,
    ExceptionHandling,
    InterceptorPhase,
    State,

    ### Additional types
    DEFAULT_LANGUAGE,
    DEFAULT_LANGUAGES,
    TargetTextsInput,
    LanguageChoice,
    LanguageCode,
    LocalizationInput,
    LocalizationsInput,
    InterceptorMethod,
    StateMask,
    StateSet,
    StateSetInput,
    SwitchNamePattern,
    SwitchSelectionInput,

    ### ProtoBuf encode/decode utility methods
    encodeLocalization,
    encodeLocalizationMap,
    encodeStateSet,
    encodeSwitchSelection,
    encodeOptionalSwitchSelection,

    SwitchboardDissecter,
    switchboard_dissecter,
)

from .base import (
    # Types from `signals.py`
    switchboard_signals,

    # Types from `switch.py`
    Switch,
    AsyncSwitch,
    InterceptorName,
    SwitchUpdateSubscriber,

    # Types from `baseboard.py`
    SwitchboardBase,

    # Types from `observer.py`
    SwitchboardObserver,
    HandlerSpec,
    MAP_UPDATE,
)

from .grpc import (
    BaseClient,
    StandardClient,
    AsyncClient,
    Client,
)
