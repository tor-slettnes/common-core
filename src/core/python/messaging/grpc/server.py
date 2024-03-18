#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file server.py
## @brief Interactive wrapper for access to Instrument Services via gRPC
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Third-party modules
import grpc

### Standard Python modules
from concurrent import futures

def create_server(*servicers):
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    for servicer in servicers:
        servicer.add_to_server(server)
    return server

def create_async_server(*servicers):
    server = grpc.aio.server(futures.ThreadPoolExecutor(max_workers=10))
    for servicer in servicers:
        servicer.add_to_server(server)
    return server
