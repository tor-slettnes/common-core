'''
__init__.py - Switchboard ProtoBuf type wrappers
'''

### Types generated from `switchboard_types.proto`
from .switchboard_types_pb2 import *

from .switchboard_types import (
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
    encodeDependency,
    encodeDependencyMap,
    encodeStateSet,
    encodeSwitchSelection,
    encodeOptionalSwitchSelection,
)

from .switchboard_dissecter import (
    SwitchboardDissecter,
    switchboard_dissecter,
)
