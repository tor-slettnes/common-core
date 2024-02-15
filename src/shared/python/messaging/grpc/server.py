#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file server.py
## @brief Interactive wrapper for access to Instrument Services via gRPC
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from concurrent import futures
import grpc

def create_server (*servicers):
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    for servicer in servicers:
        servicer.add_to_server(server)
    return server
