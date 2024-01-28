#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file grpc_status.py
## @brief Wrapper for gRPC-based services, common to client & server
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import grpc
from event_types_pb2 import Details

class DetailedError (grpc.RpcError):
    '''Mix-in class for gRPC exceptions'''
    _GRPC_DETAILS_METADATA_KEY = 'grpc-status-details-bin'

    def __init__ (self, exception):
        self.exception         = exception
        self.code              = exception.code
        self.details           = exception.details
        self.trailing_metadata = exception.trailing_metadata

    def __repr__ (self):
        return self.__str__()

    def __str__ (self):
        return "[%s] %s\n%s%s"%(type(self.exception).__name__,
                                self.details(),
                                self.summary(),
                                self.custom_details())

    def stacktrace (self):
        tb = self.exception.__traceback__
        msg = [ "  %s:%d:%s(): %s\n"%(filepath, lineno, method, text)
                for filepath, lineno, method, text in traceback.extract_tb(tb)]
        return "".join(msg)

    def summary (self):
        info = [ ("name", self.exception.code().name),
                 ("code", self.code().value[0]),
                 ("text", self.code().value[1]),
                 ("details", self.details()) ]

        return "".join(["%s: %s\n"%item for item in info])

    def custom_details (self):
        for (key, value) in self.trailing_metadata():
            if key == self._GRPC_DETAILS_METADATA_KEY:
                return Details.FromString(value)
        else:
            return ""


