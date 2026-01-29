'''
Python base client for `NetConfig` gRPC service.
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard python modules
from collections.abc import Mapping
from collections import namedtuple

### Common Core modules
from cc.core.decorators import doc_inherit
from cc.core.types import HEX8
from cc.protobuf.wellknown import empty, StringValue, BoolValue
from cc.messaging.grpc.client import GenericClient

### NetConfig modules
from ..protobuf import (
    MappingKey, GlobalData, RadioState, IPConfigData,
    DeviceMap, ConnectionData, ConnectionMap,
    ActiveConnectionData, ActiveConnectionMap,
    ActiveConnectionState, ActiveConnectionStateReason,
    WiredConnectionData, WirelessConnectionData,
    AccessPointData, AccessPointMap,
    WirelessConnectionData, WEPData, WPAData, EAPData,
    IPConfigMethod, WirelessMode, KeyManagement,
)

ActiveConnectionStateTuple = namedtuple(
    "ActiveConnectionState",
    ("state", "flags", "reason"))


#===============================================================================
# BaseClient class

class BaseClient (GenericClient):
    '''
    SysConfig service client.

    Methods in this module are simple wrappers around corresponding gRPC calls,
    whose responses are returned unmodified. Depending on the gRPC channel type,
    this may be an `asyncio` coroutine that must be awaited.

    Likely you will not use this module directly, but rather one of the derived
    classes `Client`, `SignalClient`, `AsyncClient`, or `AsyncSignalClient`.
    '''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `cc.messaging.grpc.GenericClient` base to instantiate `self.stub`.
    from .netconfig_service_pb2_grpc import NetConfigStub as Stub

    def get_hostname(self) -> StringValue:
        '''
        Return the primary hostname
        '''
        return self.stub.GetHostName(empty)

    def set_hostname(self, hostname: str):
        '''
        Set the primary hostname
        '''
        request = StringValue(value=hostname)
        self.stub.SetHostname(request)

    def get_global_data(self) -> GlobalData:
        '''
        Get global network state information
        '''
        return self.stub.GetGlobalData(empty)

    def get_connections(self) -> ConnectionMap:
        '''
        Get a map of available network connections
        '''
        return self.stub.GetConnections(empty)

    def define_connection(
            self,
            id: str,
            data: WiredConnectionData|WirelessConnectionData|None = None,
            interface: str|None = None,
            ip4config: IPConfigData = IPConfigData(method = IPConfigMethod.AUTO),
            ip6config: IPConfigData = IPConfigData(method = IPConfigMethod.AUTO)):

        '''
        Add or update a network connection profile.

        Parameters:
        @param id
            A unique ID for this connection, e.g. "Wired Connection" or a
            wireless SSID. Any existing connection with this ID is replaced.

        @param data
           Either a `WiredConnectionData` or `WirelessConnectionData`
           instance, containing media-specific settings.

        @param interface
           The network interface name to which this connection applies, e.g.,
           "eth0" "wlan0". If omitted, it is deduced from available Ethernet or
           WiFi interfaces based on the type of the `data` parameter.

        @param ip4config
           IPv4 settings. If omitted, automatic IPv4 configuration is assuemd.

        @param ip6config
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

        return self.stub.DefineConnection(request)


    def remove_connection(self, key: str) -> BoolValue:
        '''
        Remove a network connection profile.

        @param key
            The ID (name) of the connection to be removed.

        @return
            Indicator of whether any connection(s) were removed.
        '''

        request = MappingKey(key=key)
        return self.stub.RemoveConnection(request)

    def activate_connection(self, key: str):
        '''
        Activate a previously-defined connection profile.

        @param key
            The ID (name) of the connection to be activated.
        '''

        request = MappingKey(key=key)
        return self.stub.ActivateConnection(request)

    def deactivate_connection(self, key: str):
        '''
        Deactivate an existing active connection.

        @param key
            The ID (name) of the connection to be deactivated.
        '''

        request = MappingKey(key=key)
        return self.stub.DeactivateConnection(request)

    def get_active_connections(self) -> ActiveConnectionMap:
        '''
        Return a dictionary mapping connection IDs to active connection data
        for currently active connections.
        '''
        return self.stub.GetActiveConnections(empty)

    def get_active_connection_state(self,
                                    key: str,
                                    connections: Mapping[str, ActiveConnectionData],
                                    ignoreMissing: bool = False,
                                    ) -> ActiveConnectionStateTuple:

        '''
        Return named tuple with information about the state of an active
        connection, specified by either its human-readable ID or universally
        unique ID (UUID):

               (state, flags, reason)

        The 'ignoreMissing' flag controls the behavior if the connection is not
        (yet) available. If True, the following triplet is then returned:

               (UNKNOWN, 0x00, UNKNOWN)

        Otherwise, a KeyError is raised.

        ### Example

          ```python
          >>> nc = NetConfigClient()
          >>> nc.get_active_connection_state('84b75e14-a3ed-4134-a2f8-a2f4c390de6a').state
          DEACTIVATED

          >>> nc.get_active_connection_state('My Connection').reason
          LOGIN_FAILED
          ```
        '''

        try:
            ac = connections[key]
        except KeyError as e:
            for ac in connections.values():
                if ac.id == key:
                    break
            else:
                if ignoreMissing:
                    return None
                else:
                    raise

        state = ActiveConnectionState(ac.state)
        flags = HEX8(ac.state_flags)
        reason = ActiveConnectionStateReason(ac.state_reason)
        return ActiveConnectionStateTuple(state, flags, reason)



    def request_scan(self):
        '''
        Request a scan for available Access Points (APs).  The scan takes
        place asynchronously, and results may be queried using `get_aps()`.
        Alternatively, use `SignalClient` to received scan updates
        asynchronously.
        '''
        return self.stub.RequestScan(empty)

    def get_aps(self) -> AccessPointMap:
        '''
        Return a dictionary mapping access point BSSIDs (MAC addresses) to
        corresponding data
        '''
        return self.stub.GetAccessPoints(empty)

    def connect_ap(self,
                   ap: str|bytes|bytearray,
                   connection: ConnectionData):
        '''
        Define and activate a new connection based on information obtained
        from the specified access point.

        @param ap
            An available Access Point, specified by either
            - a string of the form "XX:XX:XX:XX:XX:XX" denoting its BSSID, or
            - a bytes instance containing its SSID ("Network Name")

        @param connection
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
               method=IPConfigMethod.MANUAL,
               address_data=[my_ip4_address],
               gateway='192.168.1.1')

          my_ip6 = cc.protobuf.netconfig.IPConfigData(
               method=IPConfigMethod.AUTO)

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

        request = WirelessConnectionData(
            ssid=ssid,
            bssid=bssid,
            connection=connection)

        return self.stub.ConnectAccessPoint(request)


    def connect(self,
                id: str,
                ap: str|bytes|bytearray|None = None,
                auth : WEPData|WPAData|EAPData|None = None,
                key_mgmt: KeyManagement = KeyManagement.EMPTY,
                hidden: bool = False,
                mode: WirelessMode = WirelessMode.INFRASTRUCTURE,
                interface: str|None = None,
                ip4config: IPConfigData = IPConfigData(method=IPConfigMethod.AUTO),
                ip6config: IPConfigData = IPConfigData(method=IPConfigMethod.AUTO)):
        '''
        Convenience wrapper around `connect_ap()`, to define and activate a
        new connection based on information obtained from the specified access
        point.

        @param id
            A unique ID for this connection, e.g. "Wired Connection" or a
            wireless SSID. Any existing connection with this ID is replaced.

        @param ap
            An available Access Point, specified by either
            - a string of the form "XX:XX:XX:XX:XX:XX" denoting its BSSID, or
            - a bytes instance containing its SSID (b'Network Name')
            If not provided, the `id.encode()` is used.

        @param auth
            Authentication data. The following data types are supported:
            - `cc.protobuf.netconfig.WEPData`
              Wireless Encryption Protocol, with up to 4 static keys
              each comprising 5 or 13 characters (or 10 or 26 hexadecimal digits)

            - `cc.protobuf.netconfig.WPAData`
              WiFi Protected Access, in the form `WPAData(psk='password')`

            - `cc.protobuf.netconfig.EAPData`
              802.1x/EAP settings for enterprise networks.

            Use `help(PROTOCOL)` for more information on each.

        @param key_mgmt
            WiFi Key Management, use `KeyManagement.values()` for choices.
            Automatically deduced for WEP and WPA authentication schemes; required
            for EAP encyption.  See `KeyManagement.values()` for choices.

        @param hidden
            Whether this network's SSID is hidden, i.e. requires explicit connection attemtps.

        @param mode
            Wireless network type; normally WirelessMode.INFRASTRUCTURE.
            See `WirelessMode.values()` for choices.

        @param interface
            The network interface name to which this connection applies, e.g., "wlan0".
            This may be omitted, in which case the first active WiFi device is used.

        @param ip4config
            IPv4 settings. If omitted, automatic IPv4 configuration is assumed.

        @param ip6config
            IPv6 settings. If omitted, automatic IPv6 configuration is assumed.

        ### Example:

          ```python
          >>> netconfig = NetConfigClient()
          >>> netconfig.connect(
                  id = 'My Network',
                  auth = cc.protobuf.netconfig.WPAData(psk='My password'))
          ```
        '''

        if ap is None:
            ap = id.encode()

        kwargs = dict(mode=mode, hidden=hidden, key_mgmt=key_mgmt)
        if isinstance(auth, WEPData):
            kwargs.update(wep = auth)
        elif isinstance(auth, WPAData):
            kwargs.update(wpa = auth)
        elif isinstance(auth, EAPData):
            kwargs.update(eap = auth)

        conn = ConnectionData(
            id=id,
            interface=interface,
            wireless_data=WirelessConnectionData(**kwargs),
            ip4config=ip4config,
            ip6config=ip6config)

        return self.connect_ap(ap, conn)

    def get_devices(self) -> DeviceMap:
        '''
        Get information about available network devices.

        @returns
            Key/Value pairs mapping device names to associated device information.
        '''
        return self.stub.GetDevices(empty)

    def set_wireless_enabled(self, enabled: bool):
        '''
        Enable or disable the WiFi radio.
        '''
        req = RadioState(wireless_enabled=enabled)
        return self.stub.SetWirelessEnabled(req)

    def set_wireless_allowed(self, allowed: bool):
        '''
        Specify whether WiFi can be enabled.
        '''
        req = google.protobuf.BoolValue(value=allowed)
        return self.stub.SetWirelessAllowed(req)



if __name__ == '__main__':
    netconfig = Client()
