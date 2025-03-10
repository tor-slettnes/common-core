#!/bin/echo Do not invoke directly.
#===============================================================================
## @file api.py
## @brief Python interface for MultiLogger
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.protobuf.wellknown import TimestampType, encodeTimestamp
from cc.protobuf.variant import encodeKeyValueMap
from cc.protobuf.status import Domain, Level, encodeLogLevel
from cc.protobuf.multilogger import Loggable, Message, Data

import cc.core.paths

### Standard Python modules
import logging
import inspect
import abc
import os.path
import pathlib
import time
import socket
import threading


class API (logging.Handler):
    '''
    MultiLogger API
    '''

    def __init__(self,
                 identity: str|None,
                 capture_python_logs: bool = False,
                 log_level: int = logging.NOTSET):

        logging.Handler.__init__(self, level=log_level)
        self.identity = identity or cc.core.paths.programName()
        self.pathbase = cc.core.paths.pythonRoot()
        self.capture_python_logs = capture_python_logs

    def open(self):
        if self.capture_python_logs:
            pylogger = logging.getLogger()
            pylogger.addHandler(self)

    def close(self):
        if self.capture_python_logs:
            pylogger = logging.getLogger()
            pylogger.removeHandler(self)

    ## Overrides `logging.Handler.emit()` iff capturing Python logs
    def emit(self, record: logging.LogRecord):
        '''
        Capture message from Python logger
        '''

        message = self.create_message(
            text = record.getMessage(),
            level = record.levelno,
            timestamp = record.created,
            source_path = record.pathname,
            source_line = record.lineno,
            function_name = record.funcName,
            log_scope = record.name,
            thread_id = record.thread,
            thread_name = record.threadName,
            task_name = getattr(record, 'taskName', None)) # Introduced in Python 3.12

        try:
            return self.submit(message)
        except Exception as e:
            self.handleError(record)

    def trace(self,
              text: str,
              stacklevel: int = 2,
              **kwargs):
        return self.log_message(Level.LEVEL_TRACE, text, stacklevel, **kwargs)

    def debug(self, text: str,
              stacklevel: int = 2,
              **kwargs):
        return self.log_message(Level.LEVEL_DEBUG, text, stacklevel, **kwargs)

    def info(self,
             text: str,
             stacklevel: int = 2,
             **kwargs):
        return self.log_message(Level.LEVEL_INFO, text, stacklevel, **kwargs)

    def notice(self,
               text: str,
               stacklevel: int = 2,
               **kwargs):
        return self.log_message(Level.LEVEL_NOTICE, text, stacklevel, **kwargs)

    def warning(self,
                text: str,
                stacklevel: int = 2,
                **kwargs):
        self.log_message(Level.LEVEL_WARNING, text, stacklevel, **kwargs)

    def error(self,
              text: str,
              stacklevel: int = 2,
              **kwargs):
        self.log_message(Level.LEVEL_ERROR, text, stacklevel, **kwargs)

    def critical(self,
                 text: str, stacklevel:
                 int = 2,
                 **kwargs):
        self.log_message(Level.LEVEL_CRITICAL, text, stacklevel, **kwargs)

    def fatal(self,
              text: str,
              stacklevel: int = 2,
              **kwargs):
        self.log_message(Level.LEVEL_FATAL, text, stacklevel, **kwargs)

    ## Log a message
    def log_message(self,
                    level: Level | int,
                    text: str,
                    stacklevel: int = 1,
                    **kwargs):
        '''
        Log a message by direct invocation,.
        '''

        frame = inspect.stack()[stacklevel]
        message = self.create_message(
            text = text,
            level = level,
            source_path = frame.filename,
            source_line = frame.lineno,
            function_name = frame.function,
            thread_id = threading.current_thread().native_id,
            **kwargs)

        return self.submit(message)

    def create_message(self, /,
                       text: str|None = None,
                       timestamp: TimestampType|None = None,
                       level: Level | int = Level.LEVEL_NONE,
                       host: str|None = None,
                       application: str|None = None,
                       log_scope: str|None = None,
                       thread_id: int|None = None,
                       thread_name: str|None = None,
                       task_name: str|None = None,
                       source_path: str|None = None,
                       source_line: int|None = None,
                       function_name: str|None = None,
                       attributes: dict|None = None,
                       **kwargs):

        if source_path and (self.pathbase in pathlib.Path(source_path).parents):
            source_path = os.path.relpath(source_path, self.pathbase)

        attributes = (attributes | kwargs) if attributes else kwargs

        return Loggable(
            message = Message(
                text          = text,
                timestamp     = encodeTimestamp(timestamp or time.time()),
                level         = encodeLogLevel(level),
                host          = host or socket.gethostname(),
                application   = application or self.identity,
                log_scope     = log_scope,
                thread_id     = thread_id,
                thread_name   = thread_name,
                task_name     = task_name,
                source_path   = source_path,
                source_line   = source_line,
                function_name = function_name,
                attributes    = encodeKeyValueMap(attributes)))

    def log_data(self, /,
                 contract_id: str,
                 attributes: dict,
                 timestamp: TimestampType|None = None,
                 **kwargs):

        self.submit(self.create_data(contract_id, attributes, timestamp, **kwargs))


    def create_data(self,
                    contract_id: str,
                    attributes: dict,
                    timestamp: TimestampType|None = None,
                    /,
                    **kwargs):

        attributes = (attributes | kwargs) if attributes else kwargs

        return Loggable(
            data = Data(
                contract_id = contract_id,
                timestamp = encodeTimestamp(timestamp or time.time()),
                attributes = encodeKeyValueMap(attributes)))

    @abc.abstractmethod
    def submit(self, message: Loggable):
        pass
