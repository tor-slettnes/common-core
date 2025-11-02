'''
Abstract control API for `MultiLogger` service
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Standard Python modules
from collections.abc import Iterator
from typing import Optional
import abc
import inspect
import logging
import os.path
import pathlib
import socket
import threading
import time

### Modules within package
from cc.protobuf.builder import MessageBuilder
from cc.protobuf.wellknown import empty
from cc.protobuf.status import Level
from cc.protobuf.datetime import Interval

### Modules relative to current module
from ..protobuf import Loggable, ListenerSpec, SinkSpec, ColumnType, ColumnSpec

class API (MessageBuilder):
    '''
    MultiLogger Control API
    '''

    def __init__(self, rpc: object):
        '''
        @param[in] rpc
            RPC stub that invokes method invocations on the MultiLogger server,
            waits, and returns the response from the server.  Each service
            method is given exactly one ProtoBuf message as input, and is
            expected to return exactly one ProtoBuf message as received from the
            server.
        '''
        MessageBuilder.__init__(self)
        self.rpc = rpc


    def listen(self,
               min_level: Level|int = Level.TRACE,
               contract_id: Optional[str] = None) -> Iterator[Loggable]:
        '''
        Listen for logged events from the MultiLogger
        '''

        spec = self.build_from_dict(
            ListenerSpec,
            locals(),
            sink_id = self.identity)

        return self.rpc.Listen(spec)


    def add_sink(self,
                 sink_id: str,
                 sink_type: str,
                 contract_id: Optional[str] = None,
                 min_level: Level = Level.NONE,
                 filename_template: Optional[str] = None,
                 rotation_interval: Optional[Interval] = None,
                 use_local_time: Optional[bool] = None,
                 compress_after_use: Optional[bool] = None,
                 expiration_interval: Optional[Interval] = None,
                 columns: None|tuple|ColumnSpec = None) -> bool:
        '''
        Add a log sink of the specified type. This will capture log events
        with `min_level` or higher priority, optionally restrictecd to those
        with a specific `contract_id`.

        @param sink_id
            Unique identity of this sink.  This cannot conflict with a sink type
            (see `list_sink_types()` for a list), as these are reserved for
            default log sinks.

        @param sink_type
            Log sink type, such as `csv` or `logfile`.

        @param contract_id
             If specified, capture only log messages with a matching
             `contract_id` field. This can be used to capture telemetry,
             where matching log events are expected to contain a specific
             set of custom attributes.

        @param min_level
            Severy threshold below which messages will be ignored.

        @param filename_template
            Output file template for log sinks that record events directly to
            files. This template is expanded to an actual filename as as
            described below.

        @param rotation_interval
            Specifies frequently a new log file is created. See below.

        @param use_local_time
            Whether to align rotation intervals to local time, as opposed to UTC.
            This also controls how timestamp are translated to strings within
            certain log sinks.

        @param compress_after_use
            Compress log files once they are no longer being recorded to.
            The compressed files will have a `.gz` suffix.

        @param expiration_interval
            Remove log files after the specified time interval. The default is
            one year.

        @param columns
             What fields to capture. Applicable only for *tabular data* (column
             oriented) sink types, currently these are "csvfile" and "sqlite3"`.

        The `filename_template` controls the naming of output files created by
        the "logfile", "csvfile", "jsonfile", and "sqlite3" sinks. Within the
        template name, the following holders are expanded:

        - `{executable}`: the stem of the executable file (like "logserver")
        - `{hostname}`  : the name of the host where the log file is created
        - `{sink_id}`   : the unique identity of the log sink
        - `{isodate}`   : date formatted as `YYYY-MM-DD`
        - `{isotime}    : time of day formatted as `HH:MM:SS`
        - `{year}`      : four-digit year
        - `{month}`     : two-digit month (01 - 12)
        - `{day}`       : two-digit day (01 - 31)
        - `{hour}`      : two-digit hour (00 - 23)
        - `{minute}`    : two-digit minute (00 - 59)
        - `{second}`    : two-digit second (00 - 61)
        - `{zonename}`  : time zone name or abbreviation (e.g., `PST`)
        - `{zoneoffset}`: time zone offset (e.g. `-0700`)

        The timestamp is the "last aligned" time point, i.e., the current time
        truncated according to the specified rotation interval. For instance, given
        the default settings `rotation: 6 hours` and `local time: true`, the
        timestamps would be aligned at 00:00, 06:00, 12:00, and 18:00, local time.

        The optional `rotation_interval` intervals may be specified in one of two ways:
        1. a preconstructed `cc.protobuf.datetime.Interval` instance, or
        2. a 2-element tuple of the form `(COUNT, UNIT)`, where `COUNT` is a
           positive integer and `UNIT` is a `cc.protobuf.datetime.TimeUnit`
           enumeration.

        Finally, the `fields` parameter determines what specific event data is
        captured by log sinks that organize event attributes into columns
        (currently those are `"csv"` and `"sqlite3"`).  The argument should be
        given as a list of 3-element tuples, each comprising:
          - The event field to capture; use `list_static_fields()` to get a
            list of standard fields (but messages may contain additional fields,
            normally indicated with `contract_id`)
          - The column name (i.e. CSV header or database field)
          - The column type, as either a `cc.protobuf.logging.ColumnType`
            enumeration, or alternatively as a Python type object (currently one
            of: `None`, `bool`, `int`, `float`, str`, `bytes`)..

        The column type argument is also used to interpret fields such as
        `level` and `timestamp` in one way or another. (Note that if the type of
        the `timestamp` field set to `TEXT`, the resulting string value is also
        determined by the Sink's `local time` setting).

        The return value indicates whether a new sink was created or not.

        ### Example:

        Log specific data fields per contract `"my_contract"` to a CSV file,
        rotated (changed) every month.

          ```python

          logger = cc.platform.multilogger.grpc.Client()
          logger.add_sink(
              'my_data_logger',
              sink_type = 'csv',
              filename_template = '{hostname}-{year}-{month}.csv',
              rotation_interval = (1, cc.protobuf.datetime.TimeUnit.MONTH),
              contract_id = 'mydatalogger',
              columns = [('timestamp', 'EpochTime', int),
                         ('timestamp', 'LocalTime', str),
                         ('level', 'SeverityLevel', str),
                         ('text', 'Message', str),
                         ('custom_field', 'CustomValue', float)]
          ```
        '''

        request = self.build_from_dict(SinkSpec, locals())
        response = self.rpc.AddSink(request)
        return response.added


    def remove_sink(self, sink_id: str) -> bool:
        '''
        Remove an existing log sink.

        The return value indicates whether a sink was actually removed.
        '''

        request = SinkID(
            sink_id = sink_id)

        return self.rpc.RemoveSink(request).removed


    def get_sink(self, sink_id: str) -> SinkSpec:
        '''
        Obtain specification for an existing sink.
        '''

        request = SinkID(
            sink_id = sink_id)

        return self.rpc.GetSink(request)

    def get_all_sinks(self) -> list[SinkSpec]:
        '''
        Retrieve specifications for all active log sinks
        '''
        return self.rpc.GetAllSinks(empty).specs

    def list_sinks(self) -> list[str]:
        '''
        List available log sinks.
        '''
        return self.rpc.ListSinks(empty).sink_names


    def list_sink_types(self) -> list[str]:
        '''
        List available log sink types
        '''
        return self.rpc.ListSinkTypes(empty).sink_types


    def list_static_fields(self) -> list[str]:
        '''
        List static fields for log messages
        '''
        return self.rpc.ListStaticFields(empty).field_names

