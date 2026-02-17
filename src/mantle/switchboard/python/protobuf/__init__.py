'''
__init__.py - Switchboard ProtoBuf type wrappers
'''

### Types generated from `switchboard_types.proto`
from .switchboard_types_pb2 import *


from .switchboard_types import (
    ### Overridden type enumerations
    CascadeStyle,
    DependencyPolarity,
    ExceptionHandling,
    InterceptorPhase,
    State,

    ### Additional types
    DEFAULT_LANGUAGE,
    DEFAULT_LANGUAGES,
    InterceptorMethod,
    LanguageChoice,
    LanguageCode,
    LocalizationInput,
    LocalizationsInput,
    StateMask,
    StateSet,
    StateSetInput,
    SwitchNamePattern,
    SwitchSelectionInput,
    TargetTextsInput,

    ### ProtoBuf encode/decode utility methods
    encodeLocalization,
    encodeLocalizationMap,
    encodeStateSet,
    encodeSwitchSelection,
    encodeOptionalSwitchSelection,
)
