#!/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief Python client for `SysConfig` gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.messaging.grpc import SignalClient
from cc.protobuf.import_proto import import_proto
from cc.protobuf.wellknown import TimestampType, encodeTimestamp, decodeTimestamp
from cc.core.invocation import check_type
from cc.protobuf.sysconfig import encodeCountry

### Third-party modules
from google.protobuf.empty_pb2 import Empty

### Standard Python modules
from typing import Optional, Sequence, Iterator

## Import generated ProtoBuf symbols. These will appear in namespaces
## corresponding to the package names from their `.proto` files:
## `google.protobuf` and `cc.platform.sysconfig`.
import_proto('sysconfig', globals())
import_proto('google.protobuf.wrappers', globals())
import_proto('google.protobuf.timestamp', globals())

#===============================================================================
# SysConfigClient class

class SysConfigClient (SignalClient):
    '''Client for SysConfig service.'''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `messaging.grpc.Client` base to instantiate `self.stub`.
    from cc.generated.sysconfig_pb2_grpc import SysConfigStub as Stub

    ## `signal_type` is used to construct a `cc.protobuf.SignalStore` instance,
    ## which serves as a clearing house for emitting and receiving messages.
    signal_type = cc.platform.sysconfig.Signal

    def get_product_info(self) -> cc.platform.sysconfig.ProductInfo:
        '''Get information about this product:
        model, serial number, versions, subsystems'''
        return self.stub.get_product_info(Empty())

    def set_serial_number(self, serial: str|int):
        '''Set the product serial number. For manufacturing use.'''
        self.stub.set_serial_number(google.protobuf.StringValue(value=str(serial)))

    def set_model_name(self, model: str):
        '''Set the product model. For manufacturing use.'''
        self.stub.set_model_name(google.protobuf.StringValue(value=model))

    def get_host_info(self) -> cc.platform.sysconfig.HostInfo:
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

    def get_current_time(self) -> float:
        '''Get the current time in native Python semantics:
        a double floating point representing seconds since UNIX epoch
        '''
        return decodeTimestamp(self.get_current_timestamp())

    def set_time_config(self, synchronization: bool, servers: Optional[Sequence[str]] = None):
        '''
        Enable or disable automatic time configuration,
        with optional NTP server list.
        '''

        request = cc.platform.sysconfig.TimeConfig(synchronization=synchronization)
        if servers is not None:
            request.servers.extend(servers)
        self.stub.set_time_config(request)

    def get_time_config(self) -> cc.platform.sysconfig.TimeConfig:
        '''Get the current time in native Python semantics:
        a double floating point representing seconds since UNIX epoch
        '''
        return self.stub.get_time_config(Empty())

    def list_timezone_areas(self) -> list[str]:
        '''Obtain a list of Time Zone "areas": continents, oceans, or the
        literal "Etc".  These are the top-level grouping and first part of
        canonical zone names such as "America/Los_Angeles".
        '''
        return self.stub.list_timezone_areas(Empty()).areas

    def list_timezone_countries(self,
                                area: str|None = None
                                ) -> list[cc.platform.sysconfig.TimeZoneCountry]:
        '''Obtain a list of Time Zone countries within a specific area.

        Each country is represented by
         - "code" - its 2-letter ISO 3166 code (e.g. "US" or "DE")
         - "name" - its name in English (e.g. "United States" or "Germany")
        '''
        request = cc.platform.sysconfig.TimeZoneArea(name = area)
        return list(self.stub.list_timezone_countries(request).countries)

    def list_timezone_regions(self,
                              country: str,
                              area: str|None = None
                                ) -> list[cc.platform.sysconfig.TimeZoneCountry]:
        '''Obtain a list of Time Zone regions within a specific country,
        specified by its 2-letter ISO 3166 code (e.g. "US") or its English name.
        '''
        request = cc.platform.sysconfig.TimeZoneLocationFilter(
            area = cc.platform.sysconfig.TimeZoneArea(name=area),
            country = encodeCountry(country))

        return list(self.stub.list_timezone_regions(request).regions)

    def list_timezone_specs(self,
                            area: str|None = None,
                            country: str|None = None
                            ) -> list[cc.platform.sysconfig.TimeZoneCanonicalSpec]:
        '''
        Return a list of canonical time zones and their specifications,
        optionally filtered by a specific area or country (specified by its
        2-letter ISO 3166 code, e.g. "US"). See `get_timezone_spec()` for
        details on what information is included.

        Results are sorted reasonably for grouping and presentation to an end
        user, as follows:

          - first by area (continents or oceans), roughly west to east
          - then alphabetical by short country code, except:
              - within in "Americas", countries in North America first, west to east

        '''

        request = cc.platform.sysconfig.TimeZoneLocationFilter(
            area = cc.platform.sysconfig.TimeZoneArea(name=area),
            country = encodeCountry(country))

        return self.stub.list_timezone_specs(request).specs


    def get_timezone_spec(self,
                          canonical_zone: str|None = None
                          ) -> cc.platform.sysconfig.TimeZoneCanonicalSpec:
        '''Obtain specifications about a specifc canonical zone.  If no zone is
        provided, obtain information about the currently configured zone.

        Information returned includes:
        * The canonical name (reflecting the provided name if any)
        * Area name (continent, ocean, or "Etc')
        * One or more countries for which this zone is applicable
          - Each country is represented by a 2-letter ISO 3166 code and its English name
          - If a country spans more than one zone, a `region` field describes
            this particular zone within that country
        * Geographical coordinates of the canonical location:
          - Latitude represented as seconds north of Equator
          - Longitude represented as seconds east of the Prime Meridian
        '''

        return self.stub.get_timezone_spec(
            cc.platform.sysconfig.TimeZoneCanonicalName(
                name = canonical_zone))

    def get_timezone_info(self,
                          canonical_zone: str|None = None,
                          time: TimestampType|None = None
                          ) -> cc.platform.sysconfig.TimeZoneInfo:
        '''
        Get effecitve time zone info for the specified canonical zone and time.
        If either value is missing, the currently configured zone and/or the
        current system time is used.

        Information returned includes:
        - `shortname`: effective zone abbreviation, e.g. `PST`, `PDT`, `CET`, `CEST`...
        - `offset`: offset from UTC in seconds, e.g. -7 * 60 * 60
        - `stdoffset`: standard offset from UTC in seconds, e.g. -8 * 60 * 60
        - `dst`: whether Daylight Savings Time / Summer Time is in efffect
        '''

        request = cc.platform.sysconfig.TimeZoneInfoRequest(
            canonical_zone = canonical_zone,
            time = time)

        return self.stub.get_timezone_info(request)


    def set_timezone_by_name(self,
                             zonename: str
                             ) -> cc.platform.sysconfig.TimeZoneInfo:
        '''
        Set the system timezone to the specified canonical name,
        e.g. `America/Los_Angeles' (not `PST` or `PDT`).
        '''

        request = cc.platform.sysconfig.TimeZoneConfig(
            canonical_zone = zonename)

        return self.stub.set_timezone(request)

    def set_timezone_by_location(self,
                                 country: str,
                                 region: str|None = None
                                 ) -> cc.platform.sysconfig.TimeZoneInfo:
        '''
        Set the system timezone based on country and, if appliicable, region
        within country.
        * `country` may be specified by its 2-character ISO 3166
          code (e.g. "US") or by its common English name (e.g. "United States").
        * `region` should be present if *and only if* the country
          contains multiple timezones.

        Examples:
        * `set_timezone_by_location(country="NO")`
        * `set_timezone_by_location(country="US", region="Pacific")`
        '''

        location = cc.platform.sysconfig.TimeZoneLocation(
            country = encodeCountry(country),
            region = region)

        request = cc.platform.sysconfig.TimeZoneConfig(
            location = location)

        return self.stub.set_timezone(request)



    def invoke_sync(self,
                    argv : Sequence[str] = (),
                    working_directory: str = None,
                    stdin : str = None) -> cc.platform.sysconfig.CommandResponse:
        '''Invoke a subprocess and wait for its completion.

        Inputs:
        - `argv`: Argument vector. The first element should be the full command path
        - `working_directory`: Working directory for the command.
        - `stdin`: Text to feed to the standard input of the command.

        Returns a `CommandResponse` instance with the outputs from the command
        (`stdout` and `stderr`) as well as its exit code.

        '''

        request = cc.platform.sysconfig.CommandInvocation(
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

        request = cc.platform.sysconfig.CommandInvocation(
            argv = argv,
            working_directory = working_directory,
            stdin = stdin)

        return self.stub.invoke_async(request).pid


    def invoke_finish(self,
                      pid: int,
                      stdin: str = None) -> cc.platform.sysconfig.CommandResponse:
        '''Wait for a asynchronous process to finish.

        Inputs:
        - `pid`: Process ID, as returned by `invoke_async()`
        - `stdin`: Text which will be piped to the command's standard input
        '''

        request = cc.platform.sysconfig.CommandContinuation(
            pid = pid,
            stdin = stdin)

        return self.stub.invoke_finish(request)


