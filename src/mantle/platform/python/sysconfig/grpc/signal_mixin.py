'''
Mix-in class to add signal listener to `SysConfig` gRPC service clients.
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Common Core modules
from cc.protobuf.wellknown import Timestamp
from cc.protobuf.version import Version, version_to_string
from cc.messaging.grpc.client import SignalClient as SignalClientBase

### SysConfig modules
from ..protobuf import (
    Signal, ProductInfo, HostInfo,
    TimeConfig, TimeZoneCanonicalSpec, TimeZoneInfo,
)

#===============================================================================
# SignalClient class

class SignalMixIn (SignalClientBase):
    '''
    Mix-in class to add signal listener to `SysConfig` gRPC service clients.

    To subscribe to one or more of these signals, connect your own callback
    handler using one of the provided `connect_*()` methods.  Additionally,
    property methods are provided to obtain recevied information from the local
    signal cache.
    '''

    ## `signal_type` is used to construct a `cc.protobuf.signal.SignalStore`
    ## instance, which serves as a clearing house for emitting and receiving
    ## messages.
    signal_type = Signal

    @property
    def product_info(self) -> ProductInfo:
        return self.signal_store.get_cached_signal(
            'product_info',
            fallback = ProductInfo)

    @property
    def product_version(self) -> Version:
        return self.product_info.release_version

    @property
    def printable_version(self) -> str:
        return version_to_string(self.product_version)

    @property
    def host_info(self) -> HostInfo:
        return self.signal_store.get_cached_signal(
            'host_info',
            fallback = HostInfo)

    @property
    def current_timestamp(self) -> Timestamp:
        return self.signal_store.get_cached_signal(
            'time',
            fallback = Timestamp)

    @property
    def time_config(self) -> TimeConfig:
        return self.signal_store.get_cached_signal(
            'time_config',
            fallback = TimeConfig)

    @property
    def timezone_info(self) -> TimeZoneInfo:
        return self.signal_store.get_cached_signal(
            'tz_info',
            fallback = TimeZoneInfo)

    @property
    def timezone_spec(self) -> TimeZoneCanonicalSpec:
        return self.signal_store.get_cached_signal(
            'tz_spec',
            fallback = TimeZoneCanonicalSpec)

