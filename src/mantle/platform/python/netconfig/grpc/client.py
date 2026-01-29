'''
Python client for `NetConfig` gRPC service.
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard Python modules
from collections.abc import Mapping

### Common Core modules
from cc.core.decorators import override

### NetConfig modules
from ..protobuf import (
    DeviceData, ConnectionData, ActiveConnectionData, AccessPointData,
)
from .base_client import BaseClient, ActiveConnectionStateTuple
from .signal_mixin import SignalMixIn


#===============================================================================
# SimpleClient

class SimpleClient (BaseClient):
    '''
    Stateless client for the NetConfig gRPC service.

    For a flavor that passively listens for, caches, and forwards update events
    (signals) from the server, see `Client`.
    '''

    @override
    def get_hostname(self) -> str:
        response = BaseClient.get_hostname(self)
        return response.value

    @override
    def get_connections(self) -> Mapping[str, ConnectionData]:
        response = BaseClient.get_connections(self)
        return {key:value for (key,value) in response.map.items()}

    @override
    def remove_connection(self, key: str) -> bool:
        response = BaseClient.remove_connection(**locals())
        return repsonse.value

    @override
    def get_active_connections(self) -> Mapping[str, ActiveConnectionData]:
        response = BaseClient.get_active_connections(self)
        return {key:value for (key,value) in response.map.items()}

    @override
    def get_active_connection_state(self,
                                    key: str,
                                    ignoreMissing: bool = False,
                                    ) -> ActiveConnectionStateTuple:

        connections = self.get_active_connections()
        return BaseClient.get_active_connection_state(**locals())

    @override
    def get_aps(self) -> Mapping[str, AccessPointData]:
        response = BaseClient.get_aps(self)
        return {key:value for (key,value) in response.map.items()}

    @override
    def get_devices(self) -> Mapping[str, DeviceData]:
        response = BaseClient.get_devices(self)
        return {key:value for (key,value) in response.map.items()}


#===============================================================================
# Client

class Client (SignalMixIn, SimpleClient):
    '''
    Client for the NetConfig gRPC service with signal listener.

    This specializes `SimpleClient` by passively listening for update events
    (signals) from the server.  To subscribe to one or more of these signals,
    connect your own callback handler using one of the provided `connect_*()`
    methods.  Additionally, property methods are provided to obtain recevied
    information from the local signal cache.
    '''
