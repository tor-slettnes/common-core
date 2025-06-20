'''
Python client for `MultiLogger` gRPC service
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

#===============================================================================

### Modules within package
from ..base.api import API
from cc.messaging.grpc import Client as BaseClient
from cc.protobuf.builder import MessageBuilder
from cc.protobuf.wellknown import empty
from cc.protobuf.status import Level, encodeLogLevel
from cc.protobuf.datetime import Interval, encodeInterval
from cc.protobuf.multilogger import Loggable, ListenerSpec, SinkSpec, \
    ColumnType, ColumnSpec
from cc.protobuf.variant import PyTaggedValueList, encodeTaggedValueList

### Third-party modules
import grpc

### Standard Python modules
from typing import Optional
from collections import deque
import enum
import logging
import os
import queue
import sys
import threading
import time

class OverflowDisposition (enum.IntEnum):
    BLOCK = 0
    DISCARD_OLDEST = 1
    DISCARD_NEWEST = 2


#===============================================================================
# MultiLogger Client

class Client (API, BaseClient, MessageBuilder):
    '''
    Client for MultiLogger service.
    '''

    from cc.generated.multilogger_pb2_grpc import MultiLoggerStub as Stub

    overflow_disposition_message = {
        OverflowDisposition.BLOCK : "Blocking",
        OverflowDisposition.DISCARD_OLDEST : "Discarding oldest message",
        OverflowDisposition.DISCARD_NEWEST : "Discarding message"
    }


    def __init__(
            self,
            host                : str = "",
            identity            : str = None,
            capture_python_logs : bool = False,
            log_level           : int = logging.NOTSET,
            wait_for_ready      : bool = False,
            streaming           : bool = True,
            queue_size          : int = 4096,
            overflow_disposition: OverflowDisposition = OverflowDisposition.DISCARD_OLDEST,
    ):
        '''
        Initializer.

        Inputs:
        @param host
            Host (resolvable name or IP address) of MultiLogger server

        @param identity
            Identity of this client (e.g. process name)

        @param capture_python_logs
            Register this instance as a Python log handler, which will
            capture messages sent via the `logging` module.

        @param log_level
            Capture threshold for the Python log handler, if any.

        @param streaming
            Open a gRPC write stream rather than invoking individual RPC calls
            per message.

        @param wait_for_ready
            If server is unavailable, keep waiting instead of failing.

        @param queue_size
            Max. number of log messages to cache locally if server is
            unavailable, before discarding.

        @param overflow_disposition
            Controls behavior when a new message is captured by the submit queue is full.
        '''

        API.__init__(self, identity,
                     capture_python_logs = capture_python_logs,
                     log_level = log_level)

        BaseClient.__init__(self,
                            host = host,
                            wait_for_ready = wait_for_ready)

        self.streaming = streaming
        self.overflow_disposition = overflow_disposition
        self.overflow_message = self.overflow_disposition_message[overflow_disposition]
        self.queue = None
        self.queue_size = queue_size
        self.reset()

    def __del__(self):
        self.close()

    def initialize(self):
        super().initialize()
        self.open()

    def deinitialize(self):
        super().deinitialize()
        self.close()

    def reset(self):
        self._full_queue_mutex = threading.Lock()
        self._writer_thread = None
        self._last_pid = None

    def open(self):
        '''
        Open the logger.
        Start streaming to the MultiLogger service.

        This creates a local message queue into which subsequent log messages
        are captured and then continuously streamed to the server.  This differs
        from the default behavior, where individual messages are submitted to
        the server via unary RPC calls.

        See also `close()`.
        '''

        if self.streaming:
            self.open_writer()
        super().open()


    def close(self):
        '''
        Close any active writer stream to the MultiLogger gRPC service.
        Any subsequent log messages will be sent via unary RPC calls.
        '''
        super().close()
        self.close_writer()


    def submit(self,
               loggable: Loggable|None,
               wait_for_ready: bool = False,
               ):
        '''
        Send an loggable item to the gRPC MultiLogger server.

        If we have previously invoked `open()` this message is queued for
        streaming via the gRPC `writer()` method, otherwise it is sent
        immediately via a unary RPC call.
        '''

        if self.is_writer_open():
            self.enqueue(loggable)
        else:
            try:
                self.stub.submit(loggable, wait_for_ready = wait_for_ready)
            except grpc.RpcError:
                print("gRPC Server not available")


    def enqueue(self, loggable: Loggable|None):
        if queue := self.queue:
            with self._full_queue_mutex:
                try:
                    queue.put_nowait(loggable)
                except queue.Full:
                    if self.overflow_disposition == OverflowDisposition.DISCARD_OLDEST:
                        try:
                            queue.get_nowait()
                        except queue.Empty:
                            pass

                    if self.overflow_disposition != OverflowDisposition.DISCARD_NEWEST:
                        queue.put(loggable)


    def is_writer_open(self) -> bool:
        '''
        Indicate whether the writer thread is active.

        @return
            `True` if a the writer thread is running, `False` otherwise.
        '''

        if t := self._writer_thread:
            return t.is_alive()
        else:
            return False


    def open_writer(self, wait_for_ready: bool = True):
        '''
        Start streaming to the MultiLogger service.

        This creates a local message queue into which subsequent log messages
        are captured and then continuously streamed to the server.  This differs
        from the default behavior, where individual messages are submitted to
        the server via unary RPC calls.

        See also `close_write_stream()`.
        '''

        if not self.is_writer_open():
            self.queue = queue.Queue(self.queue_size)
            t = self._writer_thread = threading.Thread(
                target = self.stream_worker,
                args = (wait_for_ready,),
                daemon = True,
            )
            t.start()

    def close_writer(self, wait: bool = False):
        '''
        Close any active writer stream to the MultiLogger gRPC service.
        Any subsequent log messages will be sent via unary RPC calls.
        '''

        q = self.queue
        t = self._writer_thread

        self.queue = None
        self._writer_thread = None

        if q:
            q.put(None)

        if wait and t and t.is_alive():
            t.join()

    def stream_worker(self, wait_for_ready: bool = True):
        '''
        Stream queued messages to MultiLogger service.
        Runs in its own thread until the queue is deleted (i.e., client is closed)
        '''

        while self.is_writer_open():
            try:
                self.stub.writer(self.queue_iterator(), wait_for_ready = wait_for_ready)
            except Exception as e:
                if self.is_writer_open():
                    logging.debug(
                        "Failed to stream message to MultiLogger service at %s: [%s]; retrying in 2s: %s"%
                        (self.host, type(e).__name__, e))
                    time.sleep(2.0)


    def queue_iterator(self):
        if q := self.queue:
            item = q.get()
            while item is not None:
                yield item
                item = q.get()


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
                 contract_id: Optional[str] = None,
                 min_level: Level = Level.LEVEL_NONE,
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

          logger = cc.multilogger.grpc.Client()
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

        response = type(self)._add_sink(**locals())
        return response.added


    def _add_sink(self, *,  min_level, rotation_interval, columns, **kwargs):
        if min_level is not None:
            kwargs.update(min_level = encodeLogLevel(min_level))

        if rotation_interval is not None:
            kwargs.update(rotation_interval = encodeInterval(rotation_interval))

        if columns:
            kwargs.update(columns = [encodeColumnSpec(column) for column in columns])

        request = SinkSpec(**kwargs)
        return self.stub.add_sink(request)


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



if __name__ == '__main__':
    client = Client(capture_python_logs = True)
    client.initialize()
    logging.info(f"MultiLogger client {client.identity} is alive!")
