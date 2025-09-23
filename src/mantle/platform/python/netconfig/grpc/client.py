'''
Python client for `NetConfig` gRPC service
'''

__all__ = ['Client', 'SignalClient']
__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard Python modules
from typing import Optional, Sequence, Union, Iterator
from enum import IntEnum
from collections import namedtuple

### Modules within package
from cc.core.scalar_types import HEX8
from cc.core.doc_inherit import doc_inherit

from cc.messaging.grpc import \
    Client as BaseClient, \
    SignalClient as BaseSignalClient

from cc.protobuf.wellknown import empty, StringValue

from cc.protobuf.netconfig import Signal, MappingKey, \
    GlobalData, RadioState, DeviceData, IPConfigData, ConnectionData, \
    ActiveConnectionData, WiredConnectionData, WirelessConnectionData, \
    AccessPointData, AccessPointConnection, WEP_Data, WPA_Data, EAP_Data, \
    IPConfigMethod, WirelessMode, KeyManagement, \
    ActiveConnectionState, ActiveConnectionStateReason

### Local types
ActiveConnectionStateTuple = namedtuple("ActiveConnectionState",
                                        ("state", "flags", "reason"))


#===============================================================================
# Client class

class Client (BaseClient):
    '''
    NetConfig service client.

    Methods in this module are wrappers around corresponding gRPC calls,
    which are forwarded to the SysConfig gRPC service.

    For a flavor that listens for update events (signals) from the server, and
    then responds to queries from the local signal cache, see `SignalClient`.
    '''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `messaging.grpc.Client` base to instantiate `self.stub`.
    from cc.generated.netconfig_pb2_grpc import NetConfigStub as Stub

    def get_hostname(self) -> str:
        '''
        Return the primary hostname
        '''
        return self.stub.get_hostname(empty).value

    def set_hostname(self, hostname: str):
        '''
        Set the primary hostname
        '''
        request = StringValue(value=hostname)
        self.stub.set_hostname(request)

    def get_global_data(self) -> GlobalData:
        '''
        Get global network state information
        '''
        return self.stub.get_global_data(empty)

    def get_connections(self) -> dict[str, ConnectionData]:
        '''
        Get a map of available network connections
        '''
        return dict(self.stub.get_connections(empty).map)

    def define_connection(
            self,
            id: str,
            data: Union[WiredConnectionData,
                        WirelessConnectionData,
                        None] = None,
            interface: Optional[str] = None,
            ip4config: IPConfigData = IPConfigData(method = IPConfigMethod.METHOD_AUTO),
            ip6config: IPConfigData = IPConfigData(method = IPConfigMethod.METHOD_AUTO)):

        '''
        Add or update a network connection profile.

        Parameters:
        @param id:
            A unique ID for this connection, e.g. "Wired Connection" or a
            wireless SSID. Any existing connection with this ID is replaced.

        @param data:
           Either a `WiredConnectionData` or `WirelessConnectionData`
           instance, containing media-specific settings.

        @param interface:
           The network interface name to which this connection applies, e.g.,
           "eth0" "wlan0". If omitted, it is deduced from available Ethernet or
           WiFi interfaces based on the type of the `data` parameter.

        @param ip4config:
           IPv4 settings. If omitted, automatic IPv4 configuration is assuemd.

        @param ip6config:
           IPv6 settings. If omitted, automatic IPv6 configuration is assuemd.
        '''

        if not interface and not data:
            raise TypeError("Either `interface` or `data` must be provided")

        elif isinstance(data, WiredConnectionData):
            request = ConnectionData(
                id=id,
                ip4config=ip4config,
                ip6config=ip6config,
                wired_data=data)

        elif isinstance(data, WirelessConnectionData):
            request = ConnectionData(
                id=id,
                ip4config=ip4config,
                ip6config=ip6config,
                wireless_data=data)

        else:
            request = ConnectionData(
                id=id,
                ip4config=ip4config,
                ip6config=ip6config)

        return self.stub.define_connection(request)


    def remove_connection(self, key: str):
        '''
        Remove a network connection profile.

        @param key:
            The ID (name) of the connection to be removed.

        Returns a boolen indicating whether any connection(s) were removed.
        '''

        request = MappingKey(key=key)
        return self.stub.remove_connection(request).value

    def activate_connection(self, key: str):
        '''
        Activate a previously-defined connection profile.

        @param key:
            The ID (name) of the connection to be activated.
        '''

        request = MappingKey(key=key)
        return self.stub.activate_connection(request)

    def deactivate_connection(self, key: str):
        '''
        Deactivate an existing active connection.

        @param key:
            The ID (name) of the connection to be deactivated.
        '''

        request = MappingKey(key=key)
        return self.stub.deactivate_connection(request)

    def get_active_connections(self) -> dict[str, ActiveConnectionData]:
        '''
        Return a dictionary mapping connection IDs to active connection data
        for currently active connections.
        '''
        return dict(self.stub.get_active_connections(empty).map)

    def request_scan(self):
        '''
        Request a scan for available Access Points (APs).  The scan takes
        place asynchronously, and any access point changes are emitted as
        signals.
        '''
        return self.stub.request_scan(empty)

    def get_aps(self) -> dict[str, AccessPointData]:
        '''
        Return a dictionary mapping access point BSSIDs (MAC addresses) to
        corresponding data
        '''

        return dict(self.stub.get_aps(empty).map)

    def get_aps_by_ssid(self) -> dict[bytes, AccessPointData]:
        '''
        Return a dictionary mapping SSIDs to the strongest corresponding
        access point.
        '''

        aps = {}
        for ap in self.get_aps().values():
            if ap.strength >= aps.get(ap.ssid, ap).strength:
                aps[ap.ssid] = ap
        return aps

    def connect_ap(self,
                   ap: Union[str, bytes, bytearray],
                   connection: ConnectionData):
        '''
        Define and activate a new connection based on information obtained
        from the specified access point.

        @param ap:
            An available Access Point, specified by either
            - a string of the form "XX:XX:XX:XX:XX:XX" denoting its BSSID, or
            - a bytes instance containing its SSID ("Network Name")

        @param connection:
            A `ConnectionData()` instance.

        ### Example

          ```python
          network = cc.protobuf.netconfig.NetworkClient()

          my_wpa = cc.protobuf.netconfig.WPA(psk='My password')

          my_wifi = cc.protobuf.netconfig.WirelessConnectionData(wpa=my_wpa)

          my_ip4_address = cc.protobuf.netconfig.AddressData(
               address='192.168.1.100',
               prefixlength=24)

          my_ip4 = cc.protobuf.netconfig.IPConfigData(
               method=IPConfigMethod.METHOD_MANUAL,
               address_data=[my_ip4_address],
               gateway='192.168.1.1')

          my_ip6 = cc.protobuf.netconfig.IPConfigData(
               method=IPConfigMethod.METHOD_AUTO)

          my_data = cc.protobuf.netconfig.ConnectionData(
               id='My Connection Name',
               wireles_data=my_wifi,
               ip4config=my_ip4,
               ip6config=my_ip6)

          netconfig.connect_ap(b'My SSID', my_data)
          ```
        '''

        if isinstance(ap, str):
            bssid, ssid = ap, None
        elif isinstance(ap, (bytes, bytearray)):
            bssid, ssid = None, ap
            if not connection.wireless_data.ssid:
                connection.wireless_data.ssid = ssid
        else:
            raise TypeError("'ap' must be a string (BSSID) or a bytearray (SSID)")

        request = AccessPointConnection(
            ssid=ssid,
            bssid=bssid,
            connection=connection)

        return self.stub.connect_ap(request)


    def connect(self,
                id: str,
                ap: str|bytes|bytearray|None = None,
                auth : Union[WEP_Data, WPA_Data, EAP_Data, None] = None,
                key_mgmt: KeyManagement = KeyManagement.KEY_EMPTY,
                hidden: bool = False,
                mode: WirelessMode = WirelessMode.MODE_INFRASTRUCTURE,
                interface: Optional[str] = None,
                ip4config: IPConfigData = IPConfigData(method=IPConfigMethod.METHOD_AUTO),
                ip6config: IPConfigData = IPConfigData(method=IPConfigMethod.METHOD_AUTO)):
        '''
        Convenience wrapper around `connect_ap()`, to define and activate a
        new connection based on information obtained from the specified access
        point.

        @param id:
            A unique ID for this connection, e.g. "Wired Connection" or a
            wireless SSID. Any existing connection with this ID is replaced.

        @param ap:
            An available Access Point, specified by either
            - a string of the form "XX:XX:XX:XX:XX:XX" denoting its BSSID, or
            - a bytes instance containing its SSID (b'Network Name')
            If not provided, the `id.encode()` is used.

        @param auth:
            Authentication data. The following data types are supported:
            - `cc.protobuf.netconfig.WEP_Data`
              Wireless Encryption Protocol, with up to 4 static keys
              each comprising 5 or 13 characters (or 10 or 26 hexadecimal digits)

            - `cc.protobuf.netconfig.WPA_Data`
              WiFi Protected Access, in the form `WPA_Data(psk='password')`

            - `cc.protobuf.netconfig.EAP_Data`
              802.1x/EAP settings for enterprise networks.

            Use `help(PROTOCOL)` for more information on each.

        @param key_mgmt:
            WiFi Key Management, use `KeyManagement.values()` for choices.
            Automatically deduced for WEP and WPA authentication schemes; required
            for EAP encyption.  See `KeyManagement.values()` for choices.

        @param hidden:
            Whether this network's SSID is hidden, i.e. requires explicit connection attemtps.

        @param mode:
            Wireless network type; normally WirelessMode.MODE_INFRASTRUCTURE.
            See `WirelessMode.values()` for choices.

        @param interface:
            The network interface name to which this connection applies, e.g., "wlan0".
            This may be omitted, in which case the first active WiFi device is used.

        @param ip4config:
            IPv4 settings. If omitted, automatic IPv4 configuration is assumed.

        @param ip6config:
            IPv6 settings. If omitted, automatic IPv6 configuration is assumed.

        ### Example:

          ```python
          >>> netconfig = NetConfigClient()
          >>> netconfig.connect(
                  id = 'My Network',
                  auth = cc.protobuf.netconfig.WPA_Data(psk='My password'))
          ```
        '''

        if ap is None:
            ap = id.encode()

        kwargs = dict(mode=mode, hidden=hidden, key_mgmt=key_mgmt)
        if isinstance(auth, WEP_Data):
            kwargs.update(wep = auth)
        elif isinstance(auth, WPA_Data):
            kwargs.update(wpa = auth)
        elif isinstance(auth, EAP_Data):
            kwargs.update(eap = auth)

        conn = ConnectionData(
            id=id,
            interface=interface,
            wireless_data=WirelessConnectionData(**kwargs),
            ip4config=ip4config,
            ip6config=ip6config)

        self.connect_ap(ap, conn)

    def get_active_connection_state(self,
                                    key: str,
                                    ignoreMissing: bool = False) -> ActiveConnectionState:

        '''
        Return named tuple with information about the state of an active
        connection, specified by either its human-readable ID or universally
        unique ID (UUID):

               (state, flags, reason)

        The 'ignoreMissing' flag controls the behavior if the connection is not
        (yet) available. If True, the following triplet is then returned:

               (CONNECTION_STATE_UNKNOWN, 0x00, CONNECTION_STATE_REASON_UNKNOWN)

        Otherwise, a KeyError is raised.

        ### Example

          ```python
          >>> nc = NetConfigClient()
          >>> nc.get_active_connection_state('84b75e14-a3ed-4134-a2f8-a2f4c390de6a').state
          CONNECTION_STATE_DEACTIVATED

          >>> nc.get_active_connection_state('My Connection').reason
          CONNECTION_STATE_REASON_LOGIN_FAILED
          ```
        '''

        acs = self.get_active_connections()

        try:
            ac = acs[key]
        except KeyError as e:
            for ac in acs.values():
                if ac.id == key:
                    break
            else:
                ac = None
                if not ignoreMissing:
                    raise

        if ac:
            state = ActiveConnectionState(ac.state)
            flags = HEX8(ac.state_flags)
            reason = ActiveConnectionStateReason(ac.state_reason)

        else:
            state = ActiveConnectionState.CONNECTION_STATE_UNKNOWN
            flags = HEX8(0)
            reason = ActiveConnectionStateReason.CONNECTION_STATE_REASON_UNKNOWN

        return ActiveConnectionStateTuple(state, flags, reason)

    def get_devices(self) -> dict[str, DeviceData]:
        '''
        Get information about available network devices.

        @returns
            Key/Value pairs mapping device names to associated device information.
        '''
        return dict(self.stub.get_devices(empty).map)


    def set_wireless_enabled(self, enabled: bool):
        '''
        Enable or disable the WiFi radio.
        '''
        req = RadioState(wireless_enabled=enabled)
        return self.stub.set_wireless_enabled(req)

    def get_wireless_enabled(self) -> bool:
        '''
        Indicate whether wireless networking is currently enabled.
        '''
        return self.get_global_data().wireless_enabled

    def get_wireless_hardware_enabled(self) -> bool:
        '''
        Indicate whether wireless networking is supported on this system.
        '''
        return self.get_global_data().wireless_hardware_enabled


    def set_wireless_allowed(self, allowed: bool):
        '''
        Specify whether WiFi can be enabled.
        '''
        req = google.protobuf.BoolValue(value=allowed)
        return self.stub.set_wireless_allowed(req)

    def get_wireless_allowed(self) -> bool:
        '''
        Indicate whether wireless networking is currently allowed.
        '''
        return self.get_global_data().wireless_allowed

#===============================================================================
# SignalClient class

class SignalClient (BaseSignalClient, Client):
    '''
    NetConfig service client.

    This specializes `Client` by passively listening for update events (signals)
    from the server.  Queries are handled locally by looking up the requested
    information in the local signal cache.
    '''

    ## `signal_type` is used to construct a `cc.protobuf.SignalStore` instance,
    ## which serves as a clearing house for emitting and receiving messages.
    signal_type = Signal

    def __init__(self, *args, **kwargs):
        BaseSignalClient.__init__(self, *args, **kwargs)
        self.start_watching(True)


    @doc_inherit
    def get_global_data(self) -> GlobalData:
        return self.signal_store.get_cached_signal(
            'global',
            fallback = GlobalData)

    @doc_inherit
    def get_devices(self) -> dict[str, DeviceData]:
        return self.signal_store.get_cached_map('device')

    @doc_inherit
    def get_connections(self) -> dict[str, ConnectionData]:
        return self.signal_store.get_cached_map('connection')

    @doc_inherit
    def get_active_connections(self) -> dict[str, ActiveConnectionData]:
        return self.signal_store.get_cached_map('active_connection')

    @doc_inherit
    def get_aps(self) -> dict[str, AccessPointData]:
        return self.signal_store.get_cached_map('accesspoint')


if __name__ == '__main__':
    netconfig = SignalClient()
