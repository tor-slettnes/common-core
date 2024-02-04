#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file error.py
## @brief ZeroMQ RPC error
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ...google.protobuf          import CC, ProtoBuf
from generated.event_types_pb2   import Details
from generated.request_reply_pb2 import Status, StatusCode, \
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

    def add_to_reply (self, reply: CC.RR.Reply):
        reply.status.code = self.code
        reply.status.details.CopyFrom(self.details)
