'''
__init__.py - Switchboard ProtoBuf type wrappers
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard Python modules
from typing import Callable, Mapping, Sequence, Set
import fnmatch
import re

### Modules within package
from cc.protobuf.utils import native_enum_from_proto

### Generated from `.../protos/cc/platform/switchboard/protobuf/switchboard_types.proto`
from .switchboard_types_pb2 import (
    Dependency, DependencyMap, DependencyPolarity, DependencyStatus,
    InterceptorPhase, InterceptorInvocation,
    ExceptionHandling, InvocationStyle, CascadeStyle, State,
    Localization, LocalizationMap, SwitchSelection,
)

# Use Python enum wrappers for some enumerated types
DependencyPolarity = native_enum_from_proto(DependencyPolarity)
InterceptorPhase   = native_enum_from_proto(InterceptorPhase)
ExceptionHandling  = native_enum_from_proto(ExceptionHandling)
InvocationStyle    = native_enum_from_proto(InvocationStyle)
CascadeStyle       = native_enum_from_proto(CascadeStyle)
State              = native_enum_from_proto(State)
StateMask          = int
StateSet           = Set[State] | Sequence[State]
StateSetInput      = StateSet | State | StateMask

SwitchName            = str
SwitchNamePattern     = str | re.Pattern
SwitchSelectionInput  = SwitchSelection | SwitchNamePattern | Sequence[SwitchNamePattern]

InterceptorMethod = Callable[[InterceptorInvocation], None]

TargetTextsInput = Mapping[State, str]

LocalizationInput = Localization | Mapping[str, object]
LocalizationsInput = LocalizationMap | Mapping[str, LocalizationInput]
DependencyInput = Dependency | Mapping[str, object]
DependenciesInput = DependencyMap | Mapping[SwitchName, DependencyInput]

LanguageCode = str
LanguageChoice = LanguageCode | Sequence[LanguageCode]

DEFAULT_LANGUAGE = "en"
DEFAULT_LANGUAGES = (DEFAULT_LANGUAGE,)


def encodeStateSet(states: StateSet,
                   output: Sequence[State] = None) -> Sequence[State]:

    if output is None:
        output = []
    else:
        output.clear()

    if isinstance(states, Sequence|Set):
        output.extend(states)

    elif isinstance(states, int):
        mask = 1
        while mask <= states:
            if (states & mask):
                output.append(mask)
            mask <<= 1

    else:
        raise TypeError("StateSet requires a bitmask or a sequence of states")

    return output


def encodeLocalization(localization : LocalizationInput|None = None,
                       description: str|None = None,
                       activate_text: str|None = None,
                       deactivate_text: str|None = None,
                       state_texts: Mapping[State, str]|None = None,
                       ) -> Localization:

    ### Start with a new copy so as not to modify the `localization` input, if any.
    result = Localization()

    if isinstance(localization, Localization):
        result.CopyFrom(localization)

    elif isinstance(localization, Mapping):
        result.MergeFrom(Localization(**localization))

    result.MergeFrom(Localization(
        description = description,
        activate_text = activate_text,
        deactivate_text = deactivate_text,
        state_texts = state_texts))

    return result


def encodeLocalizationMap(localizations: LocalizationsInput) -> LocalizationMap:
    if isinstance(localizations, LocalizationMap):
        return localizations

    elif isinstance(localizations, Mapping):
        return LocalizationMap(
            map = {language:encodeLocalization(localization)
                   for (language, localization) in localizations.items()})

    else:
        raise TypeError(
            "Expected a 'LocalizationMap' message or a mapping, got %s"%(
                type(localizations).__qualname__,
            ))


def encodeDependency(dependency : DependencyInput|None = None,
                     trigger_states: StateSet|None = None,
                     polarity: DependencyPolarity|None = None,
                     hard: bool|None = None,
                     sufficient: bool|None = None,
                     automatic: bool|None = None,
                     output: Dependency|None = None):
    '''
    Create a Dependency message
    '''

    if output is None:
        output = Dependency()
    else:
        output.Clear()


    if isinstance(dependency, Dependency):
        output.CopyFrom(dependency)

    elif isinstance(dependency, Mapping):
        output.MergeFrom(Dependency(**dependency))

    if trigger_states is not None:
        encodeStateSet(trigger_states, output.trigger_states)
    elif automatic is not None:
        encodeStateSet(State.SETTLED if automatic else 0, output.trigger_states)

    output.MergeFrom(Dependency(
        polarity = polarity,
        hard = hard,
        sufficient = sufficient))

    return output


def encodeDependencyMap(dependencies: DependenciesInput) -> DependencyMap:
    if isinstance(dependencies, DependencyMap):
        return dependencies

    elif isinstance(dependencies, Mapping):
        return DependencyMap(
            map = {predecessor:encodeDependency(dependency)
                   for (predecessor, dependency) in dependencies.items()})

    else:
        raise TypeError(
            "Expected a 'DependencyMap' message or a mapping, got %s"%(
                type(dependencies).__qualname__,
            ))


def encodeOptionalSwitchSelection(patterns: SwitchSelectionInput|None,
                                  with_ancestors: bool|None = None,
                                  ) -> SwitchSelection|None:

    if patterns is None:
        return None
    else:
        return encodeSwitchSelection(patterns, with_ancestors)


def encodeSwitchSelection(patterns: SwitchSelectionInput,
                          with_ancestors: bool|None = None,
                          ) -> SwitchSelection:

    if isinstance(patterns, SwitchSelection):
        return patterns

    if isinstance(patterns, SwitchNamePattern) or not isinstance(patterns, Sequence):
        patterns = [patterns]

    use_regex = False
    for pat in patterns:
        if isinstance(pat, re.Pattern):
            use_regex = True

        elif not isinstance(pat, str):
            raise TypeError(
                "Patterns must be of type '%s.%s' or a sequence of "
                "'str' or 're.Pattern' objects; got '%s': %s" %(
                    __package__,
                    SwitchSelection.__name__,
                    type(pat).__qualname__,
                    pat,
                ))


    if use_regex:
        patterns = [pat.pattern if isinstance(pat, re.Pattern)
                    else fnmatch.translate(pat)
                    for pat in patterns]


    return SwitchSelection(
        patterns = patterns,
        is_regex = use_regex,
        with_ancestors = with_ancestors,
    )


