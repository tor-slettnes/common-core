'''
Python client for `SysConfig` gRPC service
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard Python modules
from typing import Sequence

### Common Core modules
from cc.core.decorators import override
from cc.core.timeutils import TimePoint
from cc.protobuf.wellknown import decodeTimestamp

### SysConfig modules
from ..protobuf import TimeZoneCanonicalSpec, TimeZoneCountry
from .base_client import BaseClient
from .signal_mixin import SignalMixIn

#===============================================================================
# SimpleClient

class SimpleClient (BaseClient):
    '''
    Stateless client for the SysConfig gRPC service.

    For a flavor that passively listens for, caches, and forwards update events
    (signals) from the server, see `Client`.
    '''

    @override
    def get_printable_version(self) -> str:
        product_info = self.get_product_info()
        return super().get_printable_version(product_info)

    @override
    def get_current_time(self) -> TimePoint:
        timestamp = BaseClient.get_current_time(self)
        return decodeTimestamp(timestamp)

    @override
    def list_timezone_areas(self) -> Sequence[str]:
        response = BaseClient.list_timezone_areas(self)
        return response.areas

    @override
    def list_timezone_countries(self,
                                area: str|None = None
                                ) -> list[TimeZoneCountry]:
        response = BaseClient.list_timezone_countries(**locals())
        return response.countries

    @override
    def list_timezone_regions(self,
                              country: str,
                              area: str|None = None
                              ) -> list[str]:
        response = BaseClient.list_timezone_regions(**locals())
        return response.regions

    @override
    def list_timezone_specs(self,
                            area: str|None = None,
                            country: str|None = None
                            ) -> list[TimeZoneCanonicalSpec]:
        response = BaseClient.list_timezone_specs(**locals())
        return response.specs

    @override
    def get_configured_timezone(self) -> str:
        return self.get_timezone_spec().name

    @override
    def get_current_timezone(self) -> str:
        return self.get_timezone_info().shortname

    @override
    def invoke_async(self,
                     argv : Sequence[str] = (),
                     working_directory: str = None,
                     stdin : str = None) -> int:
        invocation_response = BaseClient.invoke_async(**locals())
        return invocation_response.pid


#===============================================================================
# Client

class Client (SignalMixIn, SimpleClient):
    '''
    Client for the SysConfig gRPC service with signal listener.

    This specializes `SimpleClient` by passively listening for update events
    (signals) from the server.  To subscribe to one or more of these signals,
    connect your own callback handler using one of the provided `connect_*()`
    methods.  Additionally, property methods are provided to obtain recevied
    information from the local signal cache.
    '''
