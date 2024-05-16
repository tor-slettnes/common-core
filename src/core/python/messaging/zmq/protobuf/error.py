#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file error.py
## @brief ZeroMQ RPC error
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
import cc.protobuf.utils
import cc.protobuf.rr
import cc.protobuf.status

class Error (RuntimeError):
    '''ZMQ RPC error'''

    def __init__(self,
                 code    : cc.protobuf.rr.StatusCode,
                 details : cc.protobuf.status.Event):

        RuntimeError.__init__(self, details.text)
        self.code    = code
        self.details = details

    def __repr__ (self):
        return "%s(code=%d, details=%s)"%(
            type(self).__name__,
            self.code,
            cc.protobuf.utils.messageToDict(self.details, use_integers_for_enums=False))

    def add_to_reply (self, reply: cc.protobuf.rr.Reply):
        reply.status.code = self.code
        reply.status.details.CopyFrom(self.details)
