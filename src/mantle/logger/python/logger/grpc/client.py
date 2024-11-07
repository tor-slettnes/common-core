#!/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief Python client for `Logger` gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from ..base.api import API
from cc.messaging.grpc import Client as BaseClient
from cc.protobuf.wellknown import empty
from cc.protobuf.status import Event, Level, encodeLogLevel
from cc.protobuf.datetime import Interval, encodeInterval
from cc.protobuf.logger import ListenerSpec, SinkType, SinkSpec
from cc.protobuf.variant import PyTaggedValueList, encodeTaggedValueList

### Standard Python modules
from typing import Optional

#===============================================================================
# Logger Client

class Client (API, BaseClient):
    '''Client for Logger service.'''

    from cc.generated.logger_pb2_grpc import LoggerStub as Stub

    def __init__(self,
                 host           : str = "",      # gRPC server
                 identity       : str = None,    # Greeter identity
                 wait_for_ready : bool = False): # Keep trying to connect

        API.__init__(self, identity)
        BaseClient.__init__(self,
                            host = host,
                            wait_for_ready = wait_for_ready)
        self.writer = None

    # def open(self):
    #     self.writer = self.stub.writer()
    #     API.open(self)

    # def close(self):
    #     API.close(self)

    def submit(self, event: Event):
        '''Send an log event to the gRPC Logger server.'''
        self.stub.submit(event)

    def listen(self,
               min_level: Level|int = Level.LEVEL_DEBUG,
               contract_id: Optional[str] = None):

        spec = ListenerSpec(
            sink_id = self.identity,
            min_level = encodeLogLevel(min_level))

        if (contract_id is not None):
            spec.contract_id = contract_id

        return self.stub.listen(spec)

    def add_sink(self,
                 sink_id: str,
                 sink_type: SinkType,
                 /,
                 permanent: bool = False,
                 filename_template: Optional[str] = None,
                 rotation_interval: Optional[Interval] = None,
                 use_local_time: Optional[bool] = None,
                 min_level: Level = Level.LEVEL_NONE,
                 contract_id: Optional[str] = None,
                 fields: Optional[PyTaggedValueList] = None) -> bool:
        '''.

        Add a log sink of the specified type. This will capture log events with
        `min_level` or higher priority, optionally restrictecd to those with a
        specific `contract_id`.

        If `permanent` is true, the server recreates the sink after future
        restarts.

        The optional `filename_template` controls the naming of the output file.
        Within this template, the following placeholders are expanded:

          - '{executable}` : then name of the log server executable
          - `{sink_id}`    : the name of the log sink
          - `{isodate}`    : date formatted as "YYYY-MM-DD"
          - `{isotime}`    : time of day formatted as "HH:MM:SS"
          - `{year}`       : four-digit year
          - `{month}`      : two-digit month (01 - 12)
          - `{day}`        : two-digit day (01 - 31)
          - `{hour}`       : two-digit hour (00 - 23)
          - `{minute}`     : two-digit minute (00 - 59)
          - `{second}`     : two-digit second (00 - 61)
          - `{zonename}`   : time zone name or abbreviation (e.g., "PST")
          - `{zoneoffset}` : time zone offset (e.g. "-0700")

        The `rotation_interval` argument determines how frequently a new log
        file is created. It should be specified either as
         - an already-encoded `cc.protobuf.logger.Interval` ProtoBuf message, or
         - as a 2-element tuple of the form `(COUNT, UNIT)`, where `COUNT` is an
           integer and `UNIT` is an enumerated `cc.protobuf.datetime.TimeUnit`
           instance.

        File rotation and expansion is based on local time if `use_local_time`
        is unspecified or true, otherwise UTC time. Internally, each sink may or
        may not log timestamps according to this setting; for instance a DB sink
        records a simple UNI Epoch based timestamp.

        Sink types that expect specific fields in the log event (currently those
        are `SINKTYPE_CSV` and `SINKTYPE_DB`) also require `fields` to specify
        which fields/columns to capture, along with their field type. This
        should be provided as a list of `(_NAME_, _TYPE_) tuples where _TYPE_ is
        either a Python type object (such as `bool`, `int`, `float` or `str`) or
        otherwise a default value (such as `True`, `1`, `3.14` or `"(none)"`).
        In such cases, `contract_id` is useful to ensure agreement between what
        fields are expected vs. what (static or custom) fields are present in
        the log event. Use `get_static_fields()` to get a list of standard
        field/column names, i.e. those present in the `cc.protobuf.status.Event`
        message.

        The return value indicates whether a new sink was created or not.

        Example:

        * Log specific data fields per contract `"my_contract"` to a CSV file,
          rotated (changed) every month.

          ```python
          logger = cc.logger.Client()
          logger.add_sink(
              'my_data_logger', cc.protobuf.logger.SinkType.CSV,
              filename_template = 'my_data-{year}-{month}.csv',
              rotation_interval = (1, cc.protobuf.logger.TimeUnit.MONTH),
              contract_id = 'mydatalogger',
              fields = [('timestamp', str),        # Timestamp as string
                        ('custom_header_1', bool), # Boolean column
                        ('custom_header_2', 1.0)]) # Real number, default=1.0
          ```
        '''

        column_defaults = [
            (key, default() if isinstance(default, type) else default)
            for key, default in fields ]

        request = SinkSpec(
            sink_id = sink_id,
            sink_type = sink_type,
            permanent = permanent,
            filename_template = filename_template,
            rotation_interval = encodeInterval(rotation_interval),
            use_local_time = use_local_time,
            min_level = encodeLogLevel(min_level),
            contract_id = contract_id,
            fields = encodeTaggedValueList(column_defaults))

        return self.stub.add_sink(request).added


    def remove_sink(self, sink_id: str) -> bool:
        '''Remove an existing log sink.

        The return value indicates whether a sink was actually removed.
        '''

        request = SinkID(
            sink_id = sink_id)

        return self.stub.remove_sink(request).removed


    def get_sink(self, sink_id: str) -> SinkSpec:
        '''Obtain specification for an existing sink.'''

        request = SinkID(
            sink_id = sink_id)

        return self.stub.get_sink(request)

    def list_sinks(self) -> list[SinkSpec]:
        '''List available log sinks along with their specifications'''
        return self.stub.list_sinks(empty).specs

    def list_static_fields(self) -> list[str]:
        '''List static fields for log messages'''
        return self.stub.list_static_fields(empty).field_names
