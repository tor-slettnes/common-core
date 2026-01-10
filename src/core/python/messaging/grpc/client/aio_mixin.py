'''
aio_client.py - Generic gRPC client base, for making service calls through a
gRPC AsyncIO channel.
'''

__author__ = "Tor Slettnes"
__docformat__ = "javadoc en"

### Third party modules
import grpc

### Modules within package
from .generic_client import GenericClient
from .signal_client import SignalClient
from .interceptors import AsyncClientInterceptor
from .readers import AbstractReader, AsyncReader

class AsyncMixIn:
    '''
    Mix-in base to add Python AsyncIO semantics to `GenericClient`.
    '''

    def create_channel(self) -> grpc.aio.Channel:
        # self._channel = grpc.aio.insecure_channel(target = self.host)
        return grpc.aio.insecure_channel(
            target = self.host,
            interceptors = [
                AsyncClientInterceptor(self.wait_for_ready,
                                       self.intercept_errors)
            ])


    def create_reader(self) -> AbstractReader:
        return AsyncReader()


class AsyncClient (AsyncMixIn, GenericClient):
    '''
    Generic gRPC client base with Python AsyncIO semantics.

    Service calls are performed within a `grpc.aio.Channel` instance,
    and as such are coroutines that should be awaited.
    '''


class AsyncSignalClient(AsyncMixIn, SignalClient):
    '''
    gRPC client with additional functionality to receive streamed Signal
    messages from a gRPC server with a corresponding `Watch()` method.

    Service calls are performed within a `grpc.aio.Channel` instance,
    and as such are coroutines that should be awaited.
    '''
