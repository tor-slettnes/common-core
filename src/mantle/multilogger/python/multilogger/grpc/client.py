#!/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief Python client for `MultiLogger` gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from ..base.api import API
from cc.messaging.grpc import Client as BaseClient
from cc.protobuf.wellknown import empty
from cc.protobuf.status import Event, Level, encodeLogLevel
from cc.protobuf.datetime import Interval, encodeInterval
from cc.protobuf.multilogger import ListenerSpec, SinkSpec, \
    ColumnType, ColumnSpec
from cc.protobuf.variant import PyTaggedValueList, encodeTaggedValueList

### Third-party modules
import grpc

### Standard Python modules
from typing import Optional
import enum
import logging
import queue
import sys
import threading

class OverflowDisposition (enum.IntEnum):
    BLOCK = 0
    DISCARD_OLDEST = 1
    DISCARD_NEWEST = 2

#===============================================================================
# MultiLogger Client

class LogClient (API, BaseClient):
    '''
    Client for MultiLogger service.
    '''

    overflow_disposition_message = {
        OverflowDisposition.BLOCK : "Blocking",
        OverflowDisposition.DISCARD_OLDEST : "Discarding oldest message",
        OverflowDisposition.DISCARD_NEWEST : "Discarding message"
    }

    from cc.generated.multilogger_pb2_grpc import LoggerStub as Stub

    def __init__(
            self,
            host           : str = "",
            identity       : str = None,
            capture_python_logs: bool = False,
            log_level      : int = logging.NOTSET,
            wait_for_ready : bool = False,
            queue_size     : int = 4096,
            overflow_disposition: OverflowDisposition = OverflowDisposition.DISCARD_OLDEST):
        '''
        Initializer.

        Inputs:
        :param host:
            Host (resolvable name or IP address) of MultiLogger server

        :param identity:
            Identity of this client (e.g. process name)

        :param capture_python_logs:
            Register this instance as a Python log handler, which will
            capture messages sent via the `logging` module.

        :param log_level:
            Capture threshold for the Python log handler, if any.

        :param wait_for_ready:
            If server is unavailable, keep waiting instead of failing.

        :param queue_size:
            Max. number of log messages to cache locally if server is
            unavailable, before discarding.

        ;param overflow_disposition:
            How to handle additional log messages once local cache is full.
        '''

        API.__init__(self, identity,
                     capture_python_logs = capture_python_logs,
                     log_level = log_level)
        BaseClient.__init__(self,
                            host = host,
                            wait_for_ready = wait_for_ready)

        self.queue = None
        self.queue_size = queue_size
        self.overflow_disposition = overflow_disposition
        self.writer_thread = None

    def __del__(self):
        self.close()

    def open(self, wait_for_ready: bool = True):
        '''
        Start streaming to the MultiLogger service.

        This creates a local message queue into which subsequent log messages
        are captured and then continuously streamed to the server.  This differs
        from the default behavior, where individual messages are submitted to
        the server via unary RPC calls.

        See also `close()`.
        '''

        if not self.queue:
            if wait_for_ready:
                self.wait_for_ready = wait_for_ready

            self.queue = queue.Queue(self.queue_size)
            self.writer_thread = threading.Thread(
                name = "LogStreamer",
                target = self.stream_worker,
                daemon = True)
            self.writer_thread.start()
            API.open(self)


    def close(self):
        '''
        Close any active writer stream to the MultiLogger gRPC service.
        Any subsequent log messages will be sent via unary RPC calls.
        '''

        if self.queue:
            self.queue.put(None)   # Allow `_queue_iterator` to break free
            self.queue = None      # Do not use queue for future messags

            self.writer_thread.join()
            self.writer_thread = None

            API.close(self)

    def submit(self, event: Event):
        '''
        Send an log event to the gRPC MultiLogger server.

        If we have previously invoked `open()` this message is queued for
        streaming via the gRPC `writer()` method, otherwise it is sent
        immediately via a unary RPC call.
        '''
        if queue := self.queue:
            self._enqueue(queue, event)

        else:
            self.stub.submit(event)

    def _enqueue(self, queue: queue.Queue, event: Event):
        retry = True

        while retry:
            try:
                queue.put_nowait(event)
                retry = False

            except queue.Full:
                if self.overflow_disposition == OverflowDisposition.BLOCK:
                    queue.put(event)
                    retry = False

                elif self.overflow_disposition == OverflowDisposition.DISCARD_OLDEST:
                    try:
                        queue.get_nowait()
                    except queue.Empty:
                        pass
                    retry = True

                elif self.overflow_disposition == OverflowDisposition.DISCARD_NEWEST:
                    retry = False


    def stream_worker(self):
        '''
        Stream queued messages to MultiLogger service.
        Runs in its own thread.
        '''
        while queue := self.queue:
            try:
                return self.stub.writer(iter(queue.get, None))
            except grpc.RpcError:
                pass

    def listen(self,
               min_level: Level|int = Level.LEVEL_TRACE,
               contract_id: Optional[str] = None):

        spec = ListenerSpec(
            sink_id = self.identity,
            min_level = encodeLogLevel(min_level))

        if (contract_id is not None):
            spec.contract_id = contract_id

        return self.stub.listen(spec)

    def add_sink(self,
                 sink_id: str,
                 sink_type: str,
                 permanent: bool = False,
                 filename_template: Optional[str] = None,
                 rotation_interval: Optional[Interval] = None,
                 use_local_time: Optional[bool] = None,
                 min_level: Level = Level.LEVEL_NONE,
                 contract_id: Optional[str] = None,
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

        @param permanent
            Recreate the sink after future restarts

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

        @param min_level
            Severy threshold below which messages will be ignored.

        @param contract_id
             If specified, capture only log messages with a matching
             `contract_id` field. This can be used to capture telemetry,
             where matching log events are expected to contain a specific
             set of custom attributes.

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

          logger = cc.multilogger.LogClient()
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


        request = SinkSpec(
            sink_id = sink_id,
            sink_type = sink_type,
            permanent = permanent,
            filename_template = filename_template,
            rotation_interval = encodeInterval(rotation_interval),
            use_local_time = use_local_time,
            min_level = encodeLogLevel(min_level),
            contract_id = contract_id,
            columns = [encodeColumnSpec(column) for column in columns or ()])

        return self.stub.add_sink(request).added


    def remove_sink(self, sink_id: str) -> bool:
        '''
        Remove an existing log sink.

        The return value indicates whether a sink was actually removed.
        '''

        request = SinkID(
            sink_id = sink_id)

        return self.stub.remove_sink(request).removed


    def get_sink(self, sink_id: str) -> SinkSpec:
        '''
        Obtain specification for an existing sink.
        '''

        request = SinkID(
            sink_id = sink_id)

        return self.stub.get_sink(request)

    def get_all_sinks(self) -> list[SinkSpec]:
        '''
        Retrieve specifications for all active log sinks
        '''
        return self.stub.get_all_sinks(empty).specs

    def list_sinks(self) -> list[str]:
        '''
        List available log sinks.
        '''
        return self.stub.list_sinks(empty).sink_names


    def list_sink_types(self) -> list[str]:
        '''
        List available log sink types
        '''
        return self.stub.list_sink_types(empty).sink_types


    def list_static_fields(self) -> list[str]:
        '''
        List static fields for log messages
        '''
        return self.stub.list_static_fields(empty).field_names
