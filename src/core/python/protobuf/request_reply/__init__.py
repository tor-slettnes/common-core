#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file rr.py
## @brief Wrapper module for `request_reply.proto`
## @author Tor Slettnes
#===============================================================================

### Generated from `.../protos/cc/protobuf/request_reply/request_reply.proto`
from ..utils import native_enum_from_proto
from .request_reply_pb2 import Request, Reply, Parameter, \
    Status, StatusCode

StatusCode = native_enum_from_proto(StatusCode)
