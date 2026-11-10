'''
Package `cc.protobuf.status` - Data pertaining to status or error reporting.

The `Error` type is
 - serialized by the server and included in the `error_details()` field of `grpc.Status()` responses after a gRPC invocation failure
 - used for asynchronous error reporting

'''

from .status import (
    level_map,
    decodeLogLevel, encodeLogLevel,
)

from .errors import (
    Error, Level, Domain,
    BaseError, ApplicationError, SystemError,
    SignalError, DeviceError, ServiceError,
    is_error, is_empty,
    encodeError, encodePossibleError,
    decodeError, raise_error,
)

