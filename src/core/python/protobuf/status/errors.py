'''
Exception types based on ProtoBuf `Error` message
'''

__docformat__ = 'javadoc en'

from ..utils import native_enum_from_proto
from ..variant import encodeKeyValueMap, decodeKeyValueMap
from ..wellknown import encodeTimestamp, decodeTimestamp

### Generated from `.../protos/cc/protobuf/status/status.proto`
from .status_pb2 import (
    Error,
    Level as _Level,
    Domain as _Domain,
)

Level = native_enum_from_proto(_Level)
Domain = native_enum_from_proto(_Domain)

class BaseError(Exception):
    def __init__(self, error: Error):
        self.error = error

    def __repr__(self):
        parts = ["[%s]"%(type(self).__name__)]
        if self.error.text:
            parts.append(self.error.text)

        if attributes := self.__flattened_attributes():
            attr_parts = [
                "%s=%r"%item
                for item in decodeKeyValueMap(attributes)
            ]
            parts.append(", ".join(attr_parts).join("()"))

        return " ".join(parts)


    def __str__(self) -> str:
        return self.error.text


    def __flattened_attributes(self):
        attributes = decodeKeyValueMap(self.error.attributes)

        if self.origin:
            attributes['origin'] = self.origin

        if self.level:
            attributes['level'] = self.level.name

        if self.code:
            attributes['code'] = self.code

        if self.symbol:
            attributes['symbol'] = self.symbol

        if tp := self.timestamp:
            attributes['timestamp'] = tp

        return attributes

    @property
    def domain(self):
        return Domain(self.error.domain)

    @property
    def origin(self):
        return self.error.origin

    @property
    def level(self):
        return Level(self.error.level)

    @property
    def code(self):
        return self.error.code

    @property
    def symbol(self):
        return self.error.symbol

    @property
    def timestamp(self):
        return decodeTimestamp(self.error.timestamp)

    @property
    def attributes(self):
        return decodeKeyValueMap(self.error.attributes)

    @property
    def text(self):
        return self.error.text


class ApplicationError (BaseError):
    pass

class SystemError (BaseError):
    pass

class SignalError (BaseError):
    pass

class DeviceError (BaseError):
    pass

class ServiceError (BaseError):
    pass



def is_empty(error: Error) -> bool:
    '''
    Indicate whether the supplied error message is empty.
    '''
    return all((
        error.code == 0,
        not error.symbol,
        not error.text,
        error.level == Level.NONE,
    ))

def is_error(error: Error) -> bool:
    '''
    Indicate whether the supplied error message contains an actual error
    '''
    return error.level >= Level.ERROR


def encodePossibleError(error: Error|Exception|str,
                        domain: Domain|None = None,
                        origin: str|None = None,
                        level: Level|None = None,
                        symbol: str|None = None,
                        code: int|None = None,
                        timestamp: TimePointInput|None = None,
                        attributes: dict|None = None,
                        output: Error|None = None,
                        ) -> Error|None:

    if error:
        return encodeError(**locals())
    else:
        return None


def encodeError(error: Error|Exception|str,
                domain: Domain|None = None,
                origin: str|None = None,
                level: Level|None = None,
                symbol: str|None = None,
                code: int|None = None,
                timestamp: TimePointInput|None = None,
                attributes: dict|None = None,
                output: Error|None = None,
                ) -> Error:


    if output is None:
        output = Error()

    if isinstance(error, Error):
        output.CopyFrom(error)

    elif isinstance(error, Exception):
        output.text = str(error)
        output.symbol = type(error).__name__

    elif isinstance(error, str):
        output.text = error

    else:
        raise TypeError(
            "Invalid 'error' type %r; expected 'Error', 'Exception', or 'str'"%(
                type(error).__name__,
            ))

    if domain is not None:
        output.domain = domain
    elif not output.domain:
        output.domain = Domain.APPLICATION

    if origin is not None:
        output.origin = origin
    elif not output.origin and domain == Domain.APPLICATION:
        output.origin = process_name()

    if level is not None:
        output.level = level
    elif not output.level:
        output.level = Level.ERROR

    if symbol is not None:
        output.symbol = symbol

    if code is not None:
        output.code = code

    if timestamp is not None:
        encodeTimestamp(timestamp, output.timestamp)

    elif not output.timestamp.ToSeconds():
        output.timestamp.GetCurrentTime()

    if attributes is not None:
        encodeKeyValueMap(attributes, output.attributes)

    return output


def decodeError(error: Error) -> BaseError:
    match error.domain:
        case Domain.APPLICATION:
            return ApplicationError(error)

        case Domain.SYSTEM:
            return SystemError(error)

        case Domain.PROCESS:
            return SignalError(error)

        case Domain.DEVICE:
            return DeviceError(error)

        case Domain.SERVICE:
            return ServiceError(error)

        case _:
            return BaseError(error)

def raise_error(error: Error):
    raise decodeError(error)


