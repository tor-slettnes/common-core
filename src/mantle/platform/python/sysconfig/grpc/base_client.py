'''
Python base client for `SysConfig` gRPC service.
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard Python modules
from typing import abstractmethod, Sequence

### Common Core modules
from cc.protobuf.version import version_to_string
from cc.messaging.grpc.client import GenericClient
from cc.protobuf.wellknown import (
    empty, StringValue,
    Timestamp, TimestampType, encodeTimestamp
)

### SysConfig modules
from ..protobuf import (
    ProductInfo, HostInfo,
    TimeConfig,
    TimeZoneCanonicalSpec, TimeZoneCanonicalSpecs, TimeZoneCanonicalName,
    TimeZoneConfig, TimeZoneInfo, TimeZoneInfoRequest,
    TimeZoneArea, TimeZoneAreas,
    TimeZoneCountry, TimeZoneCountries, TimeZoneRegions,
    TimeZoneLocation, TimeZoneLocationFilter,
    CommandInvocation, CommandInvocationResponse,
    CommandContinuation, CommandResponse,
    encodeCountry,
)


#===============================================================================
# Client class

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
    ## `cc.messaging.grpc.client.GenericClient` base to instantiate `self.stub`.
    from .sysconfig_service_pb2_grpc import SysConfigStub as Stub


    def get_product_info(self) -> ProductInfo:
        '''
        Get information about this product:
        model, serial number, versions, subsystems
        '''
        return self.stub.GetProductInfo(empty)


    def get_printable_version(self,
                              product_info: ProductInfo) -> str:
        '''
        Get version number as a printable string.

        @param[in] product_info
            Previously-obtained response from `get_product_info()`, if any.

        '''
        return version_to_string(product_info.release_version)


    def set_serial_number(self, serial: str|int):
        '''
        Set the product serial number. For manufacturing use.
        '''

        input = StringValue(value=str(serial))
        return self.stub.SetSerialNumber(input)

    def set_model_name(self, model: str):
        '''
        Set the product model. For manufacturing use.
        '''
        input = StringValue(value=model)
        return self.stub.SetModelName(input)

    def get_host_info(self) -> HostInfo:
        '''
        Get information about this host: hostname, OS, hardware.
        '''
        return self.stub.GetHostInfo(empty)

    def set_host_name(self, name: str):
        '''
        Set the host name.
        '''
        input = StringValue(value=name)
        return self.stub.SetHostName(input)

    def set_current_time(self, timestamp: TimestampType):
        '''
        Set the current time. Not available with automatic time
        configuration.
        '''

        input = encodeTimestamp(timestamp)
        return self.stub.SetCurrentTime(input)

    def get_current_time(self) -> Timestamp:
        '''
        Get the current time from the server.
        '''
        return self.stub.GetCurrentTime(empty)


    def set_time_config(self,
                        synchronization: bool,
                        servers: Sequence[str]|None = None):
        '''
        Enable or disable automatic time configuration, with optional NTP
        server list.
        '''

        input = TimeConfig(synchronization=synchronization)
        if servers is not None:
            input.servers.extend(servers)
        return self.stub.SetTimeConfig(input)

    def get_time_config(self) -> TimeConfig:
        '''
        Get time configuration settings from the server.
        '''
        return self.stub.GetTimeConfig(empty)

    def list_timezone_areas(self) -> TimeZoneAreas:
        '''
        Obtain a list of Time Zone "areas": continents, oceans, or the
        literal "Etc".  These are the top-level grouping and first part of
        canonical zone names such as "America/Los_Angeles".
        '''
        return self.stub.ListTimezoneAreas(empty)

    def list_timezone_countries(self,
                                area: str|None = None
                                ) -> TimeZoneCountries:
        '''
        Obtain a list of Time Zone countries, optionally within a specific area.

        @param area
            Restrict listing to a specified global area, such as "Europe" or
            "Indian Ocean".

        @returns
            Country list, comprising
             - `code` - its 2-letter ISO 3166 code (e.g. "US" or "DE")
             - `name` - its name in English (e.g. "United States" or "Germany")
        '''
        input = TimeZoneArea(name = area)
        return self.stub.ListTimezoneCountries(input)

    def list_timezone_regions(self,
                              country: str,
                              area: str|None = None
                              ) -> TimeZoneRegions:
        '''
        Obtain a list of Time Zone regions within a specific country,
        specified by its 2-letter ISO 3166 code (e.g. "US") or its English name.
        '''
        input = TimeZoneLocationFilter(
            area = TimeZoneArea(name=area),
            country = encodeCountry(country))

        return self.stub.ListTimezoneRegions(input)

    def list_timezone_specs(self,
                            area: str|None = None,
                            country: str|None = None
                            ) -> TimeZoneCanonicalSpecs:
        '''
        Return a list of canonical time zones and their specifications.
        See `get_timezone_spec()` for details on what information is included.

        @param area
            Filter listing by a specific global area, such as "Americas"
        @param country
            Filter listing by a specific country, specified by its 2-letter ISO
            3166 code, e.g. "US", or by its English name, e.g. "United States".
        @returns
            Canonical time zones and their specifications.  Results are sorted
            reasonably for grouping and presentation to an end user:
             - first by area (continents or oceans), roughly west to east
             - then alphabetical by short country code, except:
               - within in "Americas", countries in North America first, west to east
        '''

        input = TimeZoneLocationFilter(
            area = TimeZoneArea(name=area),
            country = encodeCountry(country))

        return self.stub.ListTimezoneSpecs(input)


    def get_configured_timezone(self) -> str:
        '''
        Obtain the currently configured timezone name, e.g. 'America/Los_Angeles'.
        '''
        return self.get_timezone_spec().name


    def get_timezone_spec(self,
                          canonical_zone: str|None = None
                          ) -> TimeZoneCanonicalSpec:
        '''
        Obtain specifications about a specifc canonical zone.  If no zone is
        provided, obtain information about the currently configured zone.

        @param canonical_zone
            Canonical zone name, e.g. 'America/Los_Angeles' (not `PST` or `PDT`)
        @returns
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

        return self.stub.GetTimezoneSpec(
            TimeZoneCanonicalName(
                name = canonical_zone))

    @abstractmethod
    def get_current_timezone(self) -> str:
        '''
        Obtain the currently effective timezone, e.g. 'PST' or 'PDT'
        '''
        return self.get_timezone_info().shortname


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
        @returns
            A `TimeZoneInfo` instance, comprising
            - `shortname`: effective zone abbreviation, e.g. `PST`, `PDT`, `CET`, `CEST`...
            - `offset`: offset from UTC in seconds, e.g. -7 * 60 * 60
            - `stdoffset`: standard offset from UTC in seconds, e.g. -8 * 60 * 60
            - `dst`: whether Daylight Savings Time / Summer Time is in efffect
        '''

        input = TimeZoneInfoRequest(
            canonical_zone = canonical_zone,
            time = time)

        return self.stub.GetTimezoneInfo(input)


    def set_timezone_by_name(self,
                             zonename: str
                             ) -> TimeZoneInfo:
        '''
        Set the system timezone to the specified canonical name,
        e.g. `America/Los_Angeles' (not `PST` or `PDT`).
        '''

        input = TimeZoneConfig(
            canonical_zone = zonename)

        return self.stub.SetTimezone(input)

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
        sysconfig = cc.platform.sysconfig.grpc.BaseClient()
        sysconfig.set_timezone_by_location(country="NO")
        sysconfig.set_timezone_by_location(country="US", region="Pacific")
        ```
        '''

        country = encodeCountry(country)
        location = TimeZoneLocation(country=country, region=region)
        input = TimeZoneConfig(location = location)

        return self.stub.SetTimezone(input)



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

        @returns
            `CommandResponse` instance with the outputs from the command
            (`stdout` and `stderr`) as well as its exit code.
        '''

        if isinstance(argv, str):
            argv = [argv]

        input = CommandInvocation(
            argv = argv,
            working_directory = working_directory,
            stdin = stdin)

        return self.stub.InvokeSync(input)


    def invoke_async(self,
                    argv : Sequence[str] = (),
                    working_directory: str = None,
                    stdin : str = None) -> CommandInvocationResponse:
        '''
        Invoke a subprocess and wait for its completion.

        @param argv
            Argument vector. The first element should be the full command path

        @param working_directory
            Working directory for the command.

        @param stdin
            Text to feed to the standard input of the command.

        @returns
            Integer representing the Process ID (PID) of the command.
            This can subsequently be passed to `invoke_finish()`.
        '''

        if isinstance(argv, str):
            argv = [argv]

        input = CommandInvocation(
            argv = argv,
            working_directory = working_directory,
            stdin = stdin)

        return self.stub.InvokeAsync(input)


    def invoke_finish(self,
                      pid: int,
                      stdin: str = None) -> CommandResponse:
        '''
        Wait for a asynchronous process to finish.

        @param pid
            Process ID, as returned by `invoke_async()`

        @param stdin
            Text which will be piped to the command's standard input

        @returns
            Response from the completed process, including exit code
            and any text printed on `stdout`/`stderr`.
        '''

        input = CommandContinuation(
            pid = pid,
            stdin = stdin)

        return self.stub.InvokeFinish(input)

