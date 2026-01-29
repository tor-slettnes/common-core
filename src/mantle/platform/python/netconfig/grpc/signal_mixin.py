'''
Mix-in class to receive passive updates from `NetConfig` gRPC service
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Common Core modules
from cc.messaging.grpc.client import SignalClient as SignalClientBase

### NetConfig modules
from ..protobuf import (
    Signal, GlobalData, DeviceData, ConnectionData, AccessPointData,
    ActiveConnectionData

)

class SignalMixIn (SignalClientBase):
    '''
    Mix-in class to add signal listener to `NetConfig` gRPC service clients.

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
    def global_data(self) -> GlobalData:
        return self.signal_store.get_cached_signal(
            'global',
            fallback = GlobalData)

    @property
    def wireless_enabled(self) -> bool:
        return self.global_data.wireless_enabled

    @property
    def wireless_hardware_enabled(self) -> bool:
        return self.global_data.wireless_hardware_enabled

    @property
    def wirelss_allowed(self) -> bool:
        return self.global_data.wireless_allowed

    @property
    def devices(self) -> dict[str, DeviceData]:
        return self.signal_store.get_cached_map('device')

    @property
    def connections(self) -> dict[str, ConnectionData]:
        return self.signal_store.get_cached_map('connection')

    @property
    def active_connections(self) -> dict[str, ActiveConnectionData]:
        return self.signal_store.get_cached_map('active_connection')

    @property
    def access_points(self) -> dict[str, AccessPointData]:
        return self.signal_store.get_cached_map('accesspoint')

    @property
    def aps_by_ssid(self) -> dict[bytes, AccessPointData]:
        aps = {}
        for ap in self.access_points.values():
            if ap.strength >= aps.get(ap.ssid, ap).strength:
                aps[ap.ssid] = ap
        return aps


