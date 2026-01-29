'''
Python AsyncCIO client for `NetConfig` gRPC service
'''

### Standard Python modules
from collections.abc import Mapping

### Common Core modules
from cc.core.decorators import override
from cc.messaging.grpc.client import AsyncMixIn

### NetConfig modules
from ..protobuf import (
    GlobalData, ConnectionData, ActiveConnectionData, AccessPointData, DeviceData,
    WiredConnectionData, WirelessConnectionData,
    KeyManagement, WirelessMode, WEPData, WPAData, EAPData,
    IPConfigData, IPConfigMethod,
)

from .base_client import BaseClient, ActiveConnectionStateTuple
from .signal_mixin import SignalMixIn


#===============================================================================
# SimpleAsyncClient class

class SimpleAsyncClient (AsyncMixIn, BaseClient):
    '''
    Stateless AsyncIO client for the NetConfig gRPC service.

    For a flavor that passively listens for, caches, and forwards update events
    (signals) from the server, see `AsyncClient`.
    '''

    @override
    async def get_hostname(self) -> str:
        response = await BaseClient.get_hostname(self)
        return response.value

    @override
    async def set_hostname(self, hostname: str):
        await BaseClient.set_hostname(**locals())

    @override
    async def get_global_data(self) -> GlobalData:
        return await BaseClient.get_global_data(self)

    @override
    async def get_connections(self) -> Mapping[str, ConnectionData]:
        response = await BaseClient.get_connections(self)
        return {key:value for (key,value) in response.map.items()}

    @override
    async def define_connection(
            self,
            id: str,
            data: WiredConnectionData|WirelessConnectionData|None = None,
            interface: str|None = None,
            ip4config: IPConfigData = IPConfigData(method = IPConfigMethod.AUTO),
            ip6config: IPConfigData = IPConfigData(method = IPConfigMethod.AUTO)):

        await BaseClient.define_connection(**locals())

    @override
    async def remove_connection(self, key: str) -> bool:
        response = await BaseClient.remove_connection(**locals())
        return response.value

    @override
    async def activate_connection(self, key: str):
        await BaseClient.activate_connection(**locals())

    @override
    async def deactivate_connection(self, key: str):
        await BaseClient.deactivate_connection(**locals())

    @override
    async def get_active_connections(self) -> Mapping[str, ActiveConnectionData]:
        response = await BaseClient.get_active_connections(self)
        return {key:value for (key,value) in response.map.items()}

    @override
    async def get_active_connection_state(self,
                                          key: str,
                                          ignoreMissing: bool = False,
                                          ) -> ActiveConnectionStateTuple:
        connections = await self.get_active_connections()
        return BaseClient.get_active_connection_state(**locals())

    @override
    async def request_scan(self):
        await BaseClient.request_scan(self)

    @override
    async def get_aps(self) -> Mapping[str, AccessPointData]:
        response = await BaseClient.get_aps(self)
        return {key:value for (key,value) in response.map.items()}

    @override
    async def get_devices(self) -> Mapping[str, DeviceData]:
        response = await BaseClient.get_devices(self)
        return {key:value for (key,value) in response.map.items()}

    @override
    async def connect_ap(self,
                         ap: str|bytes|bytearray,
                         connection: ConnectionData):
        await BaseClient.connect_ap(**locals())

    @override
    async def connect(
            self,
            id: str,
            ap: str|bytes|bytearray|None = None,
            auth : WEPData|WPAData|EAPData|None = None,
            key_mgmt: KeyManagement = KeyManagement.EMPTY,
            hidden: bool = False,
            mode: WirelessMode = WirelessMode.INFRASTRUCTURE,
            interface: str|None = None,
            ip4config: IPConfigData = IPConfigData(method=IPConfigMethod.AUTO),
            ip6config: IPConfigData = IPConfigData(method=IPConfigMethod.AUTO)):

        await BaseClient.connect(**locals())


    @override
    async def set_wireless_enabled(self, enabled: bool):
        await BaseClient.set_wireless_enabled(**locals())

    @override
    async def set_wireless_allowed(self, allowed: bool):
        await BaseClient.set_wireless_allowed(**locals())




#===============================================================================
## AsyncClient

class AsyncClient (SignalMixIn, SimpleAsyncClient):
    '''
    AsyncIO client for the gRPC NetConfig service with signal listener.

    This specializes `SimpleAsyncClient` by passively listening for update
    events (signals) from the server.  To subscribe to one or more of these
    signals, connect your own callback handler using one of the provided
    `connect_*()` methods.  Additionally, property methods are provided to
    obtain recevied information from the local signal cache.
    '''
