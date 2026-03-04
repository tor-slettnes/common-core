
from enum import Enum
from typing import Mapping, Sequence, Set
from dataclasses import is_dataclass, make_dataclass, asdict
from .enumeration import EnumValue

def symbolize_value(value: object) -> object:
    '''
    Recursively translate enumerated values to their symbolic names.
    '''

    if isinstance(value, (Enum, EnumValue)):
        return value.name
    elif isinstance(value, Mapping):
        return symbolize_map(value)
    elif isinstance(value, Set):
        return symbolize_sequence(value)
    elif isinstance(value, Sequence) and not isinstance(value, str):
        return symbolize_sequence(value)
    elif is_dataclass(value):
        return symbolize_dataclass(value)
    else:
        return value

def symbolize_map(input: Mapping) -> dict:
    return {key:symbolize_value(value) for (key, value) in input.items()}

def symbolize_set(input: Set) -> set:
    return {symbolize(value) for value in input}

def symbolize_sequence(input: Sequence) -> list:
    return [symbolize(value) for value in input]

def symbolize_dataclass(input, object) -> object:
    return make_dataclass(
        cls_name = type(input).__name__,
        fields = symbolize_map(asdict(input)).items(),
    )
