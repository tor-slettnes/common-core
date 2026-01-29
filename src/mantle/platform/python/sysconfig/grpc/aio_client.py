'''
Python AsyncCIO client for `SysConfig` gRPC service
'''

### Standard Python modules
from collections.abc import Sequence

### Common Core modules
from cc.core.decorators import override
from cc.core.timeutils import TimePoint
from cc.protobuf.version import version_to_string
from cc.protobuf.wellknown import TimestampType, decodeTimestamp
from cc.messaging.grpc.client import AsyncMixIn

### SysConfig modules
from ..protobuf import (
    ProductInfo, HostInfo,
    TimeConfig,
    TimeZoneAreas, TimeZoneCountries, TimeZoneRegions,
    TimeZoneCanonicalSpec, TimeZoneInfo,
    CommandInvocationResponse, CommandResponse,
)

from .base_client import BaseClient
from .signal_mixin import SignalMixIn

#===============================================================================
## SimpleAsyncClient

class SimpleAsyncClient (AsyncMixIn, BaseClient):
    '''
    Stateless AsyncIO client for the SysConfig gRPC service.

    For a flavor that passively listens for, caches, and forwards update events
    (signals) from the server, see `AsyncClient`.
    '''

    @override
    async def get_product_info(self) -> ProductInfo:
        return await BaseClient.get_product_info(**locals())

    @override
    async def get_printable_version(self) -> str:
        product_info = await self.get_product_info()
        return BaseClient.get_printable_version(self, product_info)

    @override
    async def set_serial_number(self, serial: str|int):
        await BaseClient.set_serial_number(**locals())

    @override
    async def set_model_name(self, model: str):
        await BaseClient.set_model_name(**locals())

    @override
    async def get_host_info(self) -> HostInfo:
        return await BaseClient.get_host_info(**locals())

    @override
    async def set_host_name(self, name: str):
        await BaseClient.set_host_name(**locals())

    @override
    async def set_current_time(self, timestamp: TimestampType):
        await BaseClient.set_current_time(**locals())

    @override
    async def get_current_time(self) -> TimePoint:
        timestamp = await BaseClient.get_current_time(**locals())
        return decodeTimestamp(timestamp)

    @override
    async def set_time_config(self,
                              synchronization: bool,
                              servers: Sequence[str]|None = None):
        await BaseClient.set_time_config(**locals())

    @override
    async def get_time_config(self) -> TimeConfig:
        return await BaseClient.get_time_config(self)

    @override
    async def list_timezone_areas(self) -> TimeZoneAreas:
        response = await BaseClient.list_timezone_areas(self)
        return response.areas

    @override
    async def list_timezone_countries(self,
                                      area: str|None = None
                                      ) -> TimeZoneCountries:
        response = await BaseClient.list_timezone_countries(**locals())
        return response.countries

    @override
    async def list_timezone_regions(self,
                                    country: str,
                                    area: str|None = None
                                    ) -> TimeZoneRegions:
        response = await BaseClient.list_timezone_regions(**locals())
        return response.regions

    @override
    async def list_timezone_specs(self,
                                  area: str|None = None,
                                  country: str|None = None
                                  ) -> Sequence[TimeZoneCanonicalSpec]:
        response = await BaseClient.list_timezone_specs(**locals())
        return response.specs


    @override
    async def get_configured_timezone(self) -> str:
        zonespec = await self.get_timezone_spec()
        return zonespec.name


    @override
    async def get_timezone_spec(self,
                                canonical_zone: str|None = None
                                ) -> TimeZoneCanonicalSpec:
        return await BaseClient.get_timezone_spec(**locals())

    @override
    async def get_current_timezone(self) -> str:
        zoneinfo = await self.get_timezone_info()
        return zoneinfo.shortname

    @override
    async def get_timezone_info(self,
                                canonical_zone: str|None = None,
                                time: TimestampType|None = None
                                ) -> TimeZoneInfo:
        return await BaseClient.get_timezone_info(**locals())

    @override
    async def set_timezone_by_name(self,
                                   zonename: str
                                   ) -> TimeZoneInfo:
        return await BaseClient.set_timezone_by_name(**locals())

    @override
    async def set_timezone_by_location(self,
                                       country: str,
                                       region: str|None = None
                                       ) -> TimeZoneInfo:
        await BaseClient.set_timezone_by_location(**locals())


    @override
    async def invoke_sync(self,
                          argv : Sequence[str] = (),
                          working_directory: str = None,
                          stdin : str = None) -> int:
        invocation_response = await BaseClient.invoke_async(**locals())
        return invocation_response.pid


    @override
    async def invoke_async(self,
                           argv : Sequence[str] = (),
                           working_directory: str = None,
                           stdin : str = None) -> CommandInvocationResponse:
        return await BaseClient.invoke_async(**locals())


    @override
    async def invoke_finish(self,
                            pid: int,
                            stdin: str = None) -> CommandResponse:
        return await BaseClient.invoke_finish(**locals())


#===============================================================================
## AsyncClient

class AsyncClient (SignalMixIn, SimpleAsyncClient):
    '''
    AsyncIO client for the gRPC SysConfig service with signal listener.

    This specializes `SimpleAsyncClient` by passively listening for update
    events (signals) from the server.  To subscribe to one or more of these
    signals, connect your own callback handler using one of the provided
    `connect_*()` methods.  Additionally, property methods are provided to
    obtain recevied information from the local signal cache.
    '''
