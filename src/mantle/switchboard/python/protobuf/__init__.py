'''
__init__.py - Switchboard ProtoBuf type wrappers
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard Python modules
from typing import Optional
from collections.abc import Mapping, Sequence

### Modules within package
from cc.protobuf.utils import native_enum_from_proto

### Generated from `.../protos/cc/platform/switchboard/protobuf/switchboard_types.proto`
from .switchboard_types_pb2 import *
from .switchboard_types_pb2 import Localization

LanguageCode = str
LanguageChoice = LanguageCode | Sequence[LanguageCode]

TargetTextsInput = Mapping[State, str]
StateTextsInput = Mapping[bool, str]

LocalizationInput = Localization | Mapping[str, object]
LocalizationsInput = LocalizationMap | Mapping[str, LocalizationInput]

DEFAULT_LANGUAGE = "en"
DEFAULT_LANGUAGES = (DEFAULT_LANGUAGE,)


# Use Python enum wrappers for some enumerated types
State              = native_enum_from_proto(State)
DependencyPolarity = native_enum_from_proto(DependencyPolarity)
InterceptorPhase   = native_enum_from_proto(InterceptorPhase)
ExceptionHandling  = native_enum_from_proto(ExceptionHandling)

def encodeLocalization(localization : Optional[Localization],
                       description: Optional[str] = None,
                       target_texts: Optional[Mapping[bool, str]] = None,
                       state_texts: Optional[Mapping[State, str]] = None,
                       ) -> Localization:

    ### Start with a new copy so as not to modify the `localization` input, if any.
    result = Localization()

    if localization:
        result.CopyFrom(localization)

    localization.MergeFrom(Localization(
        description = description,
        target_texts = target_texts,
        state_texts = state_texts))

    return localization


def encodeLocalizationMap(localizations: LocalizationsInput) -> LocalizationMap:
    if isinstance(localizations, LocalizationMap):
        return localizations

    elif isinstance(localizations, Mapping):
        return LocalizationsMap(
            map = {language:localization
                   for (language, localization) in localizations.items()})

    else:
        raise TypeError("Expected a 'LocalizationMap()' message or a dictinary, not %s"%
                        (type(localizations).__name__))
