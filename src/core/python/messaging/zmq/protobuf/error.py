#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file error.py
## @brief ZeroMQ RPC error
## @author Tor Slettnes
#===============================================================================

### Modules within package
from cc.protobuf.request_reply import Reply, StatusCode
from cc.protobuf.status import Error as CommonError
from cc.protobuf.dissecter import message_dissecter

class Error (RuntimeError):
    '''
    ZMQ RPC error
    '''

    def __init__(self,
                 code    : StatusCode,
                 details : CommonError):

        RuntimeError.__init__(self, details.text)
        self.code    = code
        self.details = details

    def __repr__ (self):
        return "%s(code=%d, details=%s)"%(
            type(self).__name__,
            self.code,
            message_dissecter.decode(self.details))

    def add_to_reply (self, reply: Reply):
        reply.status.code = self.code
        reply.status.details.CopyFrom(self.details)
