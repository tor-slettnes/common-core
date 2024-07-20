#!/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief Python client for `System` gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.messaging.grpc import SignalClient
from cc.protobuf.import_proto import import_proto
from cc.protobuf.wellknown import TimestampType, encodeTimestamp, decodeTimestamp
from cc.core.invocation import check_type

### Third-party modules
from google.protobuf.empty_pb2 import Empty

### Standard Python modules
from typing import Optional, Sequence, Iterator

## Import generated ProtoBuf symbols. These will appear in namespaces
## corresponding to the package names from their `.proto` files:
## `google.protobuf` and `cc.platform.system`.
import_proto('system', globals())
import_proto('google.protobuf.wrappers', globals())
import_proto('google.protobuf.timestamp', globals())

#===============================================================================
# SystemClient class

class SystemClient (SignalClient):
    '''Client for System service.'''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `messaging.grpc.Client` base to instantiate `self.stub`.
    from cc.generated.system_pb2_grpc import SystemStub as Stub

    ## `signal_type` is used to construct a `cc.protobuf.SignalStore` instance,
    ## which serves as a clearing house for emitting and receiving messages.
    signal_type = cc.platform.system.Signal

    def get_product_info(self) -> cc.platform.system.ProductInfo:
        '''Get information about this product:
        model, serial number, versions, subsystems'''
        return self.stub.get_product_info(Empty())

    def set_serial_number(self, serial: str|int):
        '''Set the product serial number. For manufacturing use.'''
        self.stub.set_serial_number(google.protobuf.StringValue(value=serial))

    def set_model_name(self, model: str):
        '''Set the product model. For manufacturing use.'''
        self.stub.set_model_name(google.protobuf.StringValue(value=model))

    def get_host_info(self) -> cc.platform.system.HostInfo:
        '''Get information about this host:
        hostname, OS, hardware.
        '''
        return self.stub.get_host_info(Empty())

    def set_host_name(self, name: str):
        '''Set the host name.'''
        self.stub.set_host_name(google.protobuf.StringValue(value=name))

    def set_current_time(self, timestamp: TimestampType):
        '''Set the current time. Not available with automatic time configuration.'''
        self.stub.set_current_time(encodeTimestamp(timestamp))

    def get_current_timestamp(self) -> google.protobuf.Timestamp:
        '''Get the current time as a `google.protobuf.Timestamp` instance'''
        return self.stub.get_current_time(Empty())

    def get_current_time(self) -> google.protobuf.Timestamp:
        '''Get the current time in native Python semantics:
        a double floating point representing seconds since UNIX epoch
        '''
        return decodeTimestamp(self.get_current_timestamp())

    def set_time_config(self, synchronization: bool, servers: Optional[Sequence[str]] = None):
        '''
        Enable or disable automatic time configuration,
        with optional NTP server list.
        '''
        request = cc.platform.system.TimeConfig(synchronization=synchronization)
        if servers is not None:
            request.servers.extend(servers)
        self.stub.set_time_config(request)

    def get_timezone_specs(self) -> Iterator[cc.platform.system.TimeZoneSpec]:
        '''
        Obtain information about all available timezones:
        zone name, continent, country, display name, longitude/latitude.

        Results are sorted as follows:
          - first by continent, west to east
          - then by country
              - west to east within North America
              - alphabetical by short country code for other continents
        '''
        for spec in self.stub.get_timezone_specs(Empty()):
            yield spec

    def get_timezone_spec(self, zonename: str|None = None) -> cc.platform.system.TimeZoneSpec:
        '''
        Obtain information about a specifc zone. See also `get_timezone_specs()`.
        If no zone is provided, obtain information about the currently configured zone.
        '''
        return self.stub.get_timezone_spec(
            cc.platform.system.TimeZoneName(
                zonename=zonename))


    def set_timezone(self,
                     zonename: str = None,
                     automatic: bool|None = None,
                     provider: str = None):
        '''
        Configure the system time zone. Inputs:

        - `zonename`: Configured zone, e.g. `America/Los_Angeles' (not `PST` or `PDT`)
        - `automatic`: Whether to automatically obtain timezone from an online service
        - `provider`: Online service provider for automatic zone configuration.
        '''

        request = cc.platform.system.TimeZoneConfig()

        if automatic is not None:
            request.automatic = automatic

        if zonename is not None:
            request.zonename = zonename

        if provider is not None:
            request.provider = provider;

        self.stub.set_timezone(request)

    def get_configured_timezone(self) -> cc.platform.system.TimeZoneConfig:
        '''
        Get current timezone configuration, including:
        - `zonename`: Configured zone, e.g. `America/Los_Angeles' (not `PST` or `PDT`)
        - `automatic`: Whether to automatically obtain timezone from an online service
        - `provider`: Online service provider for automatic zone configuration.
        '''
        return self.stub.get_configured_timezone(Empty())


    def get_current_timezone(self) -> cc.platform.system.TimeZoneInfo:
        '''
        Get current/effecitve time zone info, including:
        - `shortname`: effective zone abbreviation, e.g. `PST`, `PDT`, `CET`, `CEST`...
        - `offset`: offset from UTC in seconds, e.g. -7 * 60 * 60
        - `stdoffset`: standard offset from UTC in seconds, e.g. -8 * 60 * 60
        - `dst`: whether Daylight Savings Time / Summer Time is in efffect
        '''
        return self.stub.get_current_timezone(Empty())

    def invoke_sync(self,
                    argv : Sequence[str] = (),
                    working_directory: str = None,
                    stdin : str = None) -> cc.platform.system.CommandResponse:
        '''Invoke a subprocess and wait for its completion.

        Inputs:
        - `argv`: Argument vector. The first element should be the full command path
        - `working_directory`: Working directory for the command.
        - `stdin`: Text to feed to the standard input of the command.

        Returns a `CommandResponse` instance with the outputs from the command
        (`stdout` and `stderr`) as well as its exit code.

        '''

        request = cc.platform.system.CommandInvocation(
            argv = argv,
            working_directory = working_directory,
            stdin = stdin)

        return self.stub.invoke_sync(request)


    def invoke_async(self,
                    argv : Sequence[str] = (),
                    working_directory: str = None,
                    stdin : str = None) -> int:
        '''
        Invoke a subprocess and wait for its completion.

        Inputs:
        - `argv`: Argument vector. The first element should be the full command path
        - `working_directory`: Working directory for the command.
        - `stdin`: Text to feed to the standard input of the command.

        Returns an integer representing the Process ID (PID) of the command.
        This can subsequently be passed to `invoke_finish()`.
        '''

        request = cc.platform.system.CommandInvocation(
            argv = argv,
            working_directory = working_directory,
            stdin = stdin)

        return self.stub.invoke_async(request).pid


    def invoke_finish(self,
                      pid: int,
                      stdin: str = None) -> cc.platform.system.CommandResponse:
        '''Wait for a asynchronous process to finish.

        Inputs:
        - `pid`: Process ID, as returned by `invoke_async()`
        - `stdin`: Text which will be piped to the command's standard input
        '''

        request = cc.platform.system.CommandInput(
            pid = pid,
            stdin = stdin)

        return self.stub.invoke_finish(request)


