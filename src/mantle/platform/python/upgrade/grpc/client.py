'''
Python clients for the `Upgrade` gRPC service
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard Python modules
from typing import Sequence

## Common Core modules
from cc.core.decorators import override

## Upgrade modules
from ..protobuf import PackageSource, PackageInfo, SourceType
from .base_client import BaseClient
from .signal_mixin import SignalMixIn

#===============================================================================
## SimpleClient

class SimpleClient (BaseClient):
    '''
    Stateless client for the Upgrade gRPC service.

    For a flavor that passively listens for, caches, and forwards update events
    (signals) from the server, see `Client`.
    '''

    @override
    def list_sources(self) -> Sequence[PackageSource]:
        response = BaseClient.list_sources(self)
        return response.sources

    @override
    def list_available(self,
                        source: SourceType|None = None
                       ) -> Sequence[PackageInfo]:
        response = BaseClient.list_available(**locals())
        return response.packages


#===============================================================================
# Client

class Client (SignalMixIn, SimpleClient):
    '''
    Client for the Upgrade gRPC service with signal listener.

    This specializes `SimpleClient` by passively listening for update events
    (signals) from the server.  To subscribe to one or more of these signals,
    connect your own callback handler using one of the provided `connect_*()`
    methods.  Additionally, property methods are provided to obtain recevied
    information from the local signal cache.
    '''
