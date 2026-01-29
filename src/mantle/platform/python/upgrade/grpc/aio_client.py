'''
Python AsyncIO base client for the `Upgrade` gRPC service
'''

__author__ = 'Tor Slettnes'

### Standard Python modules
from typing import Sequence

## Common Core modules
from cc.core.decorators import override
from cc.messaging.grpc import AsyncMixIn

## Upgrade modules
from ..protobuf import (
    PackageCatalogue, PackageSource, PackageInfo, SourceType,
)
from .base_client import BaseClient
from .signal_mixin import SignalMixIn

#===============================================================================
## SimpleAsyncClient

class SimpleAsyncClient (AsyncMixIn, BaseClient):
    '''
    Stateless AsyncIO client for the Upgrade gRPC service.

    For a flavor that passively listens for, caches, and forwards update events
    (signals) from the server, see `AsyncClient`.
    '''

    @override
    async def scan(self, source: SourceType|None = None) -> PackageCatalogue:
        return await BaseClient.scan(**locals())

    @override
    async def list_sources(self) -> Sequence[PackageSource]:
        response = await BaseClient.list_sources(self)
        return response.sources

    @override
    async def list_available(self,
                        source: SourceType|None = None
                       ) -> Sequence[PackageInfo]:
        response = await BaseClient.list_available(**locals())
        return response.packages

    @override
    async def best_available(self,
                             source: SourceType|None = None
                             ) -> PackageInfo:
        return await BaseClient.best_available(**locals())

    @override
    async def install(self,
                      source_file: SourceType|None = None,
                      force: bool = False,
                      ) -> PackageInfo:
        return await BaseClient.install(**locals())

    @override
    async def finalize(self):
        await BaseClient.finalize(self)


#===============================================================================
# AsyncClient class

class AsyncClient (SignalMixIn, SimpleAsyncClient):
    '''
    AsyncIO client for the gRPC Upgrade service with signal listener.

    This specializes `SimpleAsyncClient` by passively listening for update
    events (signals) from the server.  To subscribe to one or more of these
    signals, connect your own callback handler using one of the provided
    `connect_*()` methods.  Additionally, property methods are provided to
    obtain recevied information from the local signal cache.
    '''
