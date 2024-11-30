#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file error.py
## @brief ZeroMQ RPC error
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.protobuf.utils import messageToDict
from cc.protobuf.rr import Reply, StatusCode
from cc.protobuf.status import Event

class Error (RuntimeError):
    '''
    ZMQ RPC error
    '''

    def __init__(self,
                 code    : StatusCode,
                 details : Event):

        RuntimeError.__init__(self, details.text)
        self.code    = code
        self.details = details

    def __repr__ (self):
        return "%s(code=%d, details=%s)"%(
            type(self).__name__,
            self.code,
            messageToDict(self.details, use_integers_for_enums=False))

    def add_to_reply (self, reply: Reply):
        reply.status.code = self.code
        reply.status.details.CopyFrom(self.details)
