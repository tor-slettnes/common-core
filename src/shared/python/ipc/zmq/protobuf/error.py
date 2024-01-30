#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file error.py
## @brief ZeroMQ RPC error
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ...google.protobuf.standard_types import ProtoBuf
from event_types_pb2                   import Details
from request_reply_pb2                 import Status, StatusCode, \
    STATUS_OK, STATUS_ACCEPTED, STATUS_INVALID, STATUS_CANCELLED, STATUS_FAILED

class Error (RuntimeError):
    '''ZMQ RPC error'''

    def __init__(self,
                 code    : StatusCode,
                 details : Details):

        RuntimeError.__init__(self, details.text)
        self.code    = code
        self.details = details

    def __repr__ (self):
        return "%s(code=%d, details=%s)"%(
            type(self).__name__,
            self.code,
            ProtoBuf.decodeToDict(self.details, use_integers_for_enums=False))
