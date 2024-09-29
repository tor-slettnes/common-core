#!/usr/bin/python3 -i
#===============================================================================
## @file client.py
## @brief Python client for `NetConfig` gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.messaging.grpc import SignalClient
from cc.protobuf.import_proto import import_proto
from cc.protobuf.wellknown import empty
from cc.core.scalar_types import HEX8
from cc.core.enumeration import Enumeration

### Standard Python modules
from typing import Optional, Sequence, Union, Iterator
import collections

## Import generated ProtoBuf symbols. These will appear in namespaces
## corresponding to the package names from their `.proto` files:
## `google.protobuf` and `cc.platform.netconfig`.
import_proto('google.protobuf.wrappers', globals())
import_proto('netconfig', globals())

### Local types
ActiveConnectionStateTuple = collections.namedtuple(
    "ActiveConnectionState", ("state", "flags", "reason"))

### Data types
GlobalData                  = cc.platform.netconfig.GlobalData
RadioState                  = cc.platform.netconfig.RadioState
ConnectionData              = cc.platform.netconfig.ConnectionData
ActiveConnectionData        = cc.platform.netconfig.ActiveConnectionData
WiredConnectionData         = cc.platform.netconfig.WiredConnectionData
WirelessConnectionData      = cc.platform.netconfig.WirelessConnectionData
AccessPointData             = cc.platform.netconfig.AccessPointData
DeviceData                  = cc.platform.netconfig.DeviceData
IPConfigData                = cc.platform.netconfig.IPConfigData
AccessPointConnection       = cc.platform.netconfig.AccessPointConnection
MappingKey                  = cc.platform.netconfig.MappingKey
WEP_Data                    = cc.platform.netconfig.WEP_Data
WPA_Data                    = cc.platform.netconfig.WPA_Data
EAP_Data                    = cc.platform.netconfig.EAP_Data

## Enumerations
IPConfigMethod              = Enumeration(cc.platform.netconfig.IPConfigMethod.items())
WirelessMode                = Enumeration(cc.platform.netconfig.WirelessMode.items())
KeyManagement               = Enumeration(cc.platform.netconfig.KeyManagement.items())
AuthenticationAlgorithm     = Enumeration(cc.platform.netconfig.AuthenticationAlgorithm.items())
AuthenticationType          = Enumeration(cc.platform.netconfig.AuthenticationType.items())
EAP_Type                    = Enumeration(cc.platform.netconfig.EAP_Type.items())
EAP_Phase2                  = Enumeration(cc.platform.netconfig.EAP_Phase2.items())
WEP_KeyType                 = Enumeration(cc.platform.netconfig.WEP_KeyType.items())
DeviceType                  = Enumeration(cc.platform.netconfig.DeviceType.items())
DeviceState                 = Enumeration(cc.platform.netconfig.DeviceState.items())
DeviceFlags                 = Enumeration(cc.platform.netconfig.DeviceFlags.items())
ConnectionType              = Enumeration(cc.platform.netconfig.ConnectionType.items())
ActiveConnectionState       = Enumeration(cc.platform.netconfig.ActiveConnectionState.items())
ActivationStateFlags        = Enumeration(cc.platform.netconfig.ActivationStateFlags.items())
ActiveConnectionStateReason = Enumeration(cc.platform.netconfig.ActiveConnectionStateReason.items())
FAST_Provisioning           = Enumeration(cc.platform.netconfig.FAST_Provisioning.items())


#===============================================================================
# NetConfigClient class

class NetConfigClient (SignalClient):
    '''Client for NetConfig service.'''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `messaging.grpc.Client` base to instantiate `self.stub`.
    from cc.generated.netconfig_pb2_grpc import NetConfigStub as Stub

    ## `signal_type` is used to construct a `cc.protobuf.SignalStore` instance,
    ## which serves as a clearing house for emitting and receiving messages.
    signal_type = cc.platform.netconfig.Signal

    Signals = (SIGNAL_GLOBAL, SIGNAL_CONN, SIGNAL_AC, SIGNAL_AP, SIGNAL_DEVICE) \
        = ('global', 'connection', 'active_connection', 'accesspoint', 'device')

    def get_hostname(self) -> str:
        '''Return the primary hostname'''
        return self.stub.get_hostname(empty).value

    def set_hostname(self, hostname: str):
        '''Set the primary hostname'''
        request = google.protobuf.StringValue(value=hostname)
        self.stub.set_hostname(request)

    def get_global_data(self) -> cc.platform.netconfig.GlobalData:
        '''Get global network state information'''
        # try:
        #     return self.signal_store.get_cached(self.SIGNAL_GLOBAL)
        # except KeyError:
        return dict(self.stub.get_global_data(empty).map)

    def get_connections(self) -> dict[str, ConnectionData]:
        '''Get a map of available network connections'''

        # try:
        #     return self.signal_store.get_cached(self.SIGNAL_CONN)
        # except KeyError:
        return dict(self.stub.get_connections(empty).map)

    def define_connection(
            self,
            id: str,
            data: Union[WiredConnectionData,
                        WirelessConnectionData,
                        None] = None,
            interface: Optional[str] = None,
            ip4config: IPConfigData = IPConfigData(method=IPConfigMethod.METHOD_AUTO),
            ip6config: IPConfigData = IPConfigData(method=IPConfigMethod.METHOD_AUTO)):
        '''Add or update a network connection profile.

        Parameters:
        @param[in] id
            A unique ID for this connection, e.g. "Wired Connection" or a
            wireless SSID. Any existing connection with this ID is replaced.

        @param[in] data
           Either a `WiredConnectionData` or `WirelessConnectionData`
           instance, containing media-specific settings.

        @param[in] interface
           The network interface name to which this connection applies, e.g.,
           "eth0" "wlan0". If omitted, it is deduced from available Ethernet or
           WiFi interfaces based on the type of the `data` parameter.

        @param[in] ip4config
           IPv4 settings. If omitted, automatic IPv4 configuration is assuemd.

        @param[in] ip6config
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

        @param[in] key
            The ID (name) of the connection to be removed.

        Returns a boolen indicating whether any connection(s) were removed.
        '''

        request = MappingKey(key=key)
        return self.stub.remove_connection(request).value

    def activate_connection(self, key: str):
        '''
        Activate a previously-defined connection profile.

        @param[in] key
            The ID (name) of the connection to be activated.
        '''

        request = MappingKey(key=key)
        return self.stub.activate_connection(request)

    def deactivate_connection(self, key: str):
        '''
        Deactivate an existing active connection.

        @param[in] key
            The ID (name) of the connection to be deactivated.
        '''

        request = MappingKey(key=key)
        return self.stub.deactivate_connection(request)

    def get_active_connections(self) -> dict[str, ActiveConnectionData]:
        '''Return a dictionary mapping connection IDs to active connection data for
        currently active connections.
        '''
        # try:
        #     return self.signal_store.get_cached(self.SIGNAL_AC)
        # except KeyError:
        return dict(self.stub.get_active_connections(empty).map)

    def request_scan(self):
        '''
        Request a scan for available Access Points (APs).  The scan takes
        place asynchronously, and any access point changes are emitted as
        signals.
        '''
        return self.stub.request_scan(empty)

    def get_aps(self) -> dict[str, AccessPointData]:
        '''Return a dictionary mapping access point BSSIDs (MAC addresses)
        to corresponding data'''

        # try:
        #     return self.signal_store.get_cached(self.SIGNAL_AP)
        # except KeyError:
        return dict(self.stub.get_aps(empty).map)

    def get_aps_by_ssid(self) -> dict[bytes, AccessPointData]:
        '''Return a dictionary mapping SSIDs to the strongest corresponding
        access point.'''

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

        Parameters:

         - `ap`: An available Access Point, specified by either
            - a string of the form "XX:XX:XX:XX:XX:XX" denoting its BSSID, or
            - a bytes instance containing its SSID ("Network Name")

         - `connection`: A `ConnectionData()` instance.

        Example:

        ```python
           network = cc.platform.netconfig.NetworkClient()

           my_wpa = cc.platform.netconfig.WPA(psk='My password')

           my_wifi = cc.platform.netconfig.WirelessConnectionData(wpa=my_wpa)

           my_ip4_address = cc.platform.netconfig.AddressData(
                                address='192.168.1.100',
                                prefixlength=24)

           my_ip4 = cc.platform.netconfig.IPConfigData(
                        method=IPConfigMethod.METHOD_MANUAL,
                        address_data=[my_ip4_address],
                        gateway='192.168.1.1')

           my_ip6 = cc.platform.netconfig.IPConfigData(
                        method=IPConfigMethod.METHOD_AUTO)

           my_data = cc.platform.netconfig.ConnectionData(
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
        Convenience wrapper around `connect_ap()`, to define and activate a new
        connection based on information obtained from the specified access
        point.

        Parameters:
        @param[in] id
            A unique ID for this connection, e.g. "Wired Connection" or a
            wireless SSID. Any existing connection with this ID is replaced.

        @param[in] ap
            An available Access Point, specified by either
            - a string of the form "XX:XX:XX:XX:XX:XX" denoting its BSSID, or
            - a bytes instance containing its SSID (b'Network Name')
            If not provided, the `id.encode()` is used.

        @param[in] auth
            Authentication data. The following data types are supported:
            - `cc.platform.netconfig.WEP_Data`
              Wireless Encryption Protocol, with up to 4 static keys
              each comprising 5 or 13 characters (or 10 or 26 hexadecimal digits)

            - `cc.platform.netconfig.WPA_Data`
              WiFi Protected Access, in the form `WPA_Data(psk='password')`

            - `cc.platform.netconfig.EAP_Data`
              802.1x/EAP settings for enterprise networks.

            Use `help(PROTOCOL)` for more information on each.

        @param[in] key_mgmt
            WiFi Key Management, use `KeyManagement.values()` for choices.
            Automatically deduced for WEP and WPA authentication schemes; required
            for EAP encyption.  See `KeyManagement.values()` for choices.

        @param[in] hidden
            Whether this network's SSID is hidden, i.e. requires explicit connection attemtps.

        @param[in] mode
            Wireless network type; normally WirelessMode.MODE_INFRASTRUCTURE.
            See `WirelessMode.values()` for choices.

        @param[in] interface
            The network interface name to which this connection applies, e.g., "wlan0".
            This may be omitted, in which case the first active WiFi device is used.

        @param[in] ip4config
            IPv4 settings. If omitted, automatic IPv4 configuration is assumed.

        @param[in] ip6config
            IPv6 settings. If omitted, automatic IPv6 configuration is assumed.

        Example:
         >>> netconfig = NetConfigClient()
         >>> netconfig.connect(id = 'My Network',
                               auth = cc.platform.netconfig.WPA_Data(psk='My password'))

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
            wireless_data=cc.platform.netconfig.WirelessConnectionData(**kwargs),
            ip4config=ip4config,
            ip6config=ip6config)

        self.connect_ap(ap, conn)

    def get_active_connection_state(self,
                                    key: str,
                                    ignoreMissing: bool = False) -> ActiveConnectionState:

        '''
        Return named tuple with information about the state of an active connection,
        specified by either its human-readable ID or universally unique ID (UUID):

               (state, flags, reason)

        The 'ignoreMissing' flag controls the behavior if the connection is not
        (yet) available. If True, the following triplet is then returned:

               (CONNECTION_STATE_UNKNOWN, 0x00, CONNECTION_STATE_REASON_UNKNOWN)

        Otherwise, a KeyError is raised.

        Example:
        @code
            >>> nc = NetConfigClient()
            >>> nc.get_active_connection_state('84b75e14-a3ed-4134-a2f8-a2f4c390de6a').state
            CONNECTION_STATE_DEACTIVATED

            >>> nc.get_active_connection_state('My Connection').reason
            CONNECTION_STATE_REASON_LOGIN_FAILED
        @endcode
        '''

        acs = self.get_active_connections()
        state = ActiveConnectionState.CONNECTION_STATE_UNKNOWN
        flags = HEX8(0)
        reason = ActiveConnectionState.CONNECTION_STATE_REASON_UNKNOWN

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
            state = ActiveConnectionState.get(
                ac.state,
                ac.state)

            flags = HEX8(ac.state_flags)

            reason = ActiveConnectionStateReason.get(
                ac.state_reason,
                ac.state_reason)

        return ActiveConnectionStateTuple(state, flags, reason)

    def get_devices(self) -> dict[str, DeviceData]:
        '''
        Return a dictionary network interface names to corresponding data.
        '''
        # try:
        #     return self.signal_store.get_cached(self.SIGNAL_DEVICE)
        # except KeyError:
        return dict(self.stub.get_devices(empty).map)

    def set_wireless_enabled(self, enabled: bool):
        '''
        Enable or disable the WiFi radio.
        '''
        req = RadioState(wireless_enabled=enabled)
        return self.stub.set_wireless_enabled(req)

    def set_wireless_allowed(self, allowed: bool):
        '''
        Specify whether WiFi can be enabled.
        '''
        req = google.protobuf.BoolValue(value=allowed)
        return self.stub.set_wireless_allowed(req)

if __name__ == '__main__':
    netconfig = NetConfigClient()
