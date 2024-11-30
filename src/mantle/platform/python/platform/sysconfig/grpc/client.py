#!/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief Python client for `SysConfig` gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.messaging.grpc import SignalClient

from cc.protobuf.wellknown import empty, StringValue, \
    Timestamp, TimestampType, encodeTimestamp, decodeTimestamp

from cc.protobuf.sysconfig import Signal, ProductInfo, HostInfo, \
    TimeConfig, TimeZoneCanonicalSpec, TimeZoneCanonicalName, \
    TimeZoneConfig, TimeZoneInfo, TimeZoneInfoRequest, \
    TimeZoneArea, TimeZoneCountry, TimeZoneLocation, TimeZoneLocationFilter, \
    CommandInvocation, CommandContinuation, CommandResponse, \
    encodeCountry

### Standard Python modules
from typing import Optional, Sequence, Iterator

#===============================================================================
# SysConfigClient class

class SysConfigClient (SignalClient):
    '''
    Client for SysConfig service.
    '''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `messaging.grpc.Client` base to instantiate `self.stub`.
    from cc.generated.sysconfig_pb2_grpc import SysConfigStub as Stub

    ## `signal_type` is used to construct a `cc.protobuf.SignalStore` instance,
    ## which serves as a clearing house for emitting and receiving messages.
    signal_type = Signal

    def get_product_info(self) -> ProductInfo:
        '''
        Get information about this product:
        model, serial number, versions, subsystems
        '''
        return self.stub.get_product_info(empty)

    def set_serial_number(self, serial: str|int):
        '''
        Set the product serial number. For manufacturing use.
        '''
        self.stub.set_serial_number(StringValue(value=str(serial)))

    def set_model_name(self, model: str):
        '''
        Set the product model. For manufacturing use.
        '''
        self.stub.set_model_name(StringValue(value=model))

    def get_host_info(self) -> HostInfo:
        '''
        Get information about this host: hostname, OS, hardware.
        '''
        return self.stub.get_host_info(empty)

    def set_host_name(self, name: str):
        '''
        Set the host name.
        '''
        self.stub.set_host_name(StringValue(value=name))

    def set_current_time(self, timestamp: TimestampType):
        '''
        Set the current time. Not available with automatic time
        configuration.
        '''
        self.stub.set_current_time(encodeTimestamp(timestamp))

    def get_current_timestamp(self) -> Timestamp:
        '''
        Get the current time as a `google.protobuf.Timestamp` instance
        '''
        return self.stub.get_current_time(empty)

    def get_current_time(self) -> float:
        '''
        Get the current time in native Python semantics: a double floating
        point representing seconds since UNIX epoch
        '''
        return decodeTimestamp(self.get_current_timestamp())

    def set_time_config(self, synchronization: bool, servers: Optional[Sequence[str]] = None):
        '''
        Enable or disable automatic time configuration, with optional NTP
        server list.
        '''

        request = TimeConfig(synchronization=synchronization)
        if servers is not None:
            request.servers.extend(servers)
        self.stub.set_time_config(request)

    def get_time_config(self) -> TimeConfig:
        '''
        Get the current time in native Python semantics: a double floating
        point representing seconds since UNIX epoch
        '''
        return self.stub.get_time_config(empty)

    def list_timezone_areas(self) -> list[str]:
        '''
        Obtain a list of Time Zone "areas": continents, oceans, or the
        literal "Etc".  These are the top-level grouping and first part of
        canonical zone names such as "America/Los_Angeles".
        '''
        return self.stub.list_timezone_areas(empty).areas

    def list_timezone_countries(self,
                                area: str|None = None
                                ) -> list[TimeZoneCountry]:
        '''
        Obtain a list of Time Zone countries, optionally within a specific area.

        @param area
            Restrict listing to a specified global area, such as "Europe" or
            "Indian Ocean".

        @return
            Country list, comprising
             - `code` - its 2-letter ISO 3166 code (e.g. "US" or "DE")
             - `name` - its name in English (e.g. "United States" or "Germany")
        '''
        request = TimeZoneArea(name = area)
        return list(self.stub.list_timezone_countries(request).countries)

    def list_timezone_regions(self,
                              country: str,
                              area: str|None = None
                                ) -> list[TimeZoneCountry]:
        '''
        Obtain a list of Time Zone regions within a specific country,
        specified by its 2-letter ISO 3166 code (e.g. "US") or its English name.
        '''
        request = TimeZoneLocationFilter(
            area = TimeZoneArea(name=area),
            country = encodeCountry(country))

        return list(self.stub.list_timezone_regions(request).regions)

    def list_timezone_specs(self,
                            area: str|None = None,
                            country: str|None = None
                            ) -> list[TimeZoneCanonicalSpec]:
        '''
        Return a list of canonical time zones and their specifications.
        See `get_timezone_spec()` for details on what information is included.

        @param area
            Filter listing by a specific global area, such as "Americas"
        @param country
            Filter listing by a specific country, specified by its 2-letter ISO
            3166 code, e.g. "US", or by its English name, e.g. "United States".
        @return
            Canonical time zones and their specifications.  Results are sorted
            reasonably for grouping and presentation to an end user:
             - first by area (continents or oceans), roughly west to east
             - then alphabetical by short country code, except:
               - within in "Americas", countries in North America first, west to east
        '''

        request = TimeZoneLocationFilter(
            area = TimeZoneArea(name=area),
            country = encodeCountry(country))

        return self.stub.list_timezone_specs(request).specs


    def get_timezone_spec(self,
                          canonical_zone: str|None = None
                          ) -> TimeZoneCanonicalSpec:
        '''
        Obtain specifications about a specifc canonical zone.  If no zone is
        provided, obtain information about the currently configured zone.

        @param canonical_zone
            Canonical zone name, e.g. 'America/Los_Angeles' (not `PST` or `PDT`)
        @return
            Zone specification, including
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
            TimeZoneCanonicalName(
                name = canonical_zone))

    def get_timezone_info(self,
                          canonical_zone: str|None = None,
                          time: TimestampType|None = None
                          ) -> TimeZoneInfo:
        '''
        Get effecitve time zone information.

        @param canonical_zone
            Optional zone name, e.g. "America/Los_Angeles" (not "PST` or "PDT").
            If missing, the currently configured zone is ued.
        @param time
            Optional timestamp for which the zone information is applicable
            (mainly to determine whether daylight savings time/summer time is in
            use).  If missing, the current system time is used.
        @return
            A `TimeZoneInfo` instance, comprising
            - `shortname`: effective zone abbreviation, e.g. `PST`, `PDT`, `CET`, `CEST`...
            - `offset`: offset from UTC in seconds, e.g. -7 * 60 * 60
            - `stdoffset`: standard offset from UTC in seconds, e.g. -8 * 60 * 60
            - `dst`: whether Daylight Savings Time / Summer Time is in efffect
        '''

        request = TimeZoneInfoRequest(
            canonical_zone = canonical_zone,
            time = time)

        return self.stub.get_timezone_info(request)


    def set_timezone_by_name(self,
                             zonename: str
                             ) -> TimeZoneInfo:
        '''
        Set the system timezone to the specified canonical name,
        e.g. `America/Los_Angeles' (not `PST` or `PDT`).
        '''

        request = TimeZoneConfig(
            canonical_zone = zonename)

        return self.stub.set_timezone(request)

    def set_timezone_by_location(self,
                                 country: str,
                                 region: str|None = None
                                 ) -> TimeZoneInfo:
        '''
        Set the system timezone based on country and, if appliicable, region
        within country.

        @param country
          may be specified by its 2-character ISO 3166 code (e.g. "US") or by
          its common English name (e.g. "United States").

        @param region
          should be present if *and only if* the country contains multiple
          timezones.

        ### Examples

        ```python
        set_timezone_by_location(country="NO")
        set_timezone_by_location(country="US", region="Pacific")
        ```
        '''

        location = TimeZoneLocation(country = encodeCountry(country),
                                    region = region)

        request = TimeZoneConfig(location = location)

        return self.stub.set_timezone(request)



    def invoke_sync(self,
                    argv : Sequence[str] = (),
                    working_directory: str = None,
                    stdin : str = None) -> CommandResponse:
        '''
        Invoke a subprocess and wait for its completion.

        @param argv
            Argument vector. The first element should be the full command path
        @param working_directory
            Working directory for the command.
        @param stdin
            Text to feed to the standard input of the command.
        @return
            `CommandResponse` instance with the outputs from the command
            (`stdout` and `stderr`) as well as its exit code.
        '''

        request = CommandInvocation(
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

        @param argv
            Argument vector. The first element should be the full command path

        @param working_directory
            Working directory for the command.

        @param stdin
            Text to feed to the standard input of the command.

        @return
            Integer representing the Process ID (PID) of the command.
            This can subsequently be passed to `invoke_finish()`.
        '''

        request = CommandInvocation(
            argv = argv,
            working_directory = working_directory,
            stdin = stdin)

        return self.stub.invoke_async(request).pid


    def invoke_finish(self,
                      pid: int,
                      stdin: str = None) -> CommandResponse:
        '''
        Wait for a asynchronous process to finish.

        @param pid
            Process ID, as returned by `invoke_async()`

        @param stdin
            Text which will be piped to the command's standard input

        @return
            Response from the completed process, including exit code
            and any text printed on `stdout`/`stderr`.
        '''

        request = CommandContinuation(
            pid = pid,
            stdin = stdin)

        return self.stub.invoke_finish(request)


