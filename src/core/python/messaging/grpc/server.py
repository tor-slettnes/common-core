#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file server.py
## @brief Interactive wrapper for access to Instrument Services via gRPC
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Standard Python modules
import concurrent
from typing import Sequence

### Third-party modules
import grpc

### Common Core modules
from cc.core.decorators import override
from cc.core.timeinterval import TimeInterval
from .base import AddressPair
from .request_handler import RequestHandler


class ServerWrapper:
    '''
    gRPC server wrapper.
    '''

    def __init__(self,
                 request_handlers: RequestHandler|Sequence[RequestHandler]|None = None,
                 max_workers: int = 10,
                 **kwargs,
                 ):

        self.server = self.create_server(
            concurrent.futures.ThreadPoolExecutor(max_workers = max_workers),
            **kwargs)

        self.request_handlers = []

        if isinstance(request_handlers, Sequence):
            for request_handler in request_handlers:
                self.add_request_handler(request_handler)

        elif isinstance(request_handlers, RequestHandler):
            self.add_request_handler(request_handlers)

        elif request_handlers:
            raise TypeError(
                "ServerWrapper requires 'request_handlers' argument(s) of type %s, got %s" % (
                    'cc.messaging.grpc.RequestHandler',
                    type(request_handlers).__name__,
                ))


    def create_server(self, *args, **kwargs) -> grpc.Server:
        return grpc.server(*args, **kwargs)

    def add_listener(self,
                     address: str,
                     ):
        self.server.add_insecure_port(bind_address)

    def add_secure_listener(self,
                            address: str,
                            credentials: grpc.ServerCredentials,
                            ):
        self.server.add_insecure_port(bind_address, credentials)

    def add_request_handler(self,
                     request_handler: RequestHandler,
                     add_listener: bool = True):
        self.request_handlers.append(request_handler)
        request_handler.add_to_server(self.server, add_listener = add_listener)

    def addresses(self) -> list[AddressPair]:
        return [request_handler.service_address for request_handler in self.request_handlers]

    def start(self):
        return self.server.start()

    def stop(self, timeout: TimeInterval|float|int|None = TimeInterval(seconds=5)):
        return self.server.stop(timeout)

    def wait_for_termination(self):
        return self.server.wait_for_termination()


class AsyncServerWrapper (ServerWrapper):
    '''
    AsyncIO gRPC server wrapper.
    '''

    @override
    def create_server(self, *args, **kwargs) -> grpc.aio.Server:
        return grpc.aio.server(*args, **kwargs)



# def create_server(*request_handlers):
#     server = grpc.server(concurrent.futures.ThreadPoolExecutor(max_workers=10))
#     for servicer in request_handlers:
#         servicer.add_to_server(server)
#     return server

# def create_async_server(*request_handlers):
#     server = grpc.aio.server(concurrent.futures.ThreadPoolExecutor(max_workers=10))
#     for servicer in request_handlers:
#         servicer.add_to_server(server)
#     return server
