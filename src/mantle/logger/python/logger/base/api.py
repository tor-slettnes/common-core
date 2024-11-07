#!/bin/echo Do not invoke directly.
#===============================================================================
## @file api.py
## @brief Logger - Python interface
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.protobuf.wellknown import TimestampType, encodeTimestamp
from cc.protobuf.variant import encodeValueList
from cc.protobuf.status import Event, Domain, Level, encodeLogLevel
from cc.protobuf.logger import SinkType

import cc.core.paths

### Standard Python modules
import logging
import inspect
import abc
import os.path
import time
import socket
import threading


class API (logging.Handler):
    '''Logger API'''

    def __init__(self,
                 identity: str|None,
                 capture_python_logs: bool = False):

        self.identity = identity or cc.core.paths.programName()
        self.pathbase = cc.core.paths.pythonRoot().as_posix()
        self.capture_python_logs = capture_python_logs

    def open(self):
        if self.capture_python_logs:
            logger = logging.getLogger()
            logger.setLevel(1)
            logger.addHandler(self)

    def trace(self,
              text: str,
              stacklevel: int = 2,
              **kwargs):
        self.log(Level.LEVEL_TRACE, text, stacklevel, **kwargs)

    def debug(self, text: str,
              stacklevel: int = 2,
              **kwargs):
        self.log(Level.LEVEL_DEBUG, text, stacklevel, **kwargs)

    def info(self,
             text: str,
             stacklevel: int = 2,
             **kwargs):
        self.log(Level.LEVEL_INFO, text, stacklevel, **kwargs)

    def notice(self,
               text: str,
               stacklevel: int = 2,
               **kwargs):
        self.log(Level.LEVEL_NOTICE, text, stacklevel, **kwargs)

    def warning(self,
                text: str,
                stacklevel: int = 2,
                **kwargs):
        self.log(Level.LEVEL_WARNING, text, stacklevel, **kwargs)

    def error(self,
              text: str,
              stacklevel: int = 2,
              **kwargs):
        self.log(Level.LEVEL_FAILED, text, stacklevel, **kwargs)

    def critical(self,
                 text: str, stacklevel:
                 int = 2,
                 **kwargs):
        self.log(Level.LEVEL_CRITICAL, text, stacklevel, **kwargs)

    def fatal(self,
              text: str,
              stacklevel: int = 2,
              **kwargs):
        self.log(Level.LEVEL_FATAL, text, stacklevel, **kwargs)

    ## Log a message
    def log(self,
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

        self.submit(message)


    ## Overrides `logging.Handler.emit()` iff capturing Python logs
    def emit(self, record: logging.LogRecord):
        '''
        Capture message from Python logger
        '''

        message = self.create_message(
            text = record.getMessage(),
            level = record.level,
            source_path = record.pathname,
            source_line = record.lineno,
            function_name = record.func,
            log_scope = record.name,
            thread_id = threading.current_thread().native_id)

        self.submit(message)


    def create_message(self, /,
                       text: str,
                       level: Level | int,
                       domain: Domain = Domain.DOMAIN_APPLICATION,
                       origin: str|None = None,
                       code: int|None = None,
                       symbol: str|None = None,
                       timestamp: TimestampType|None = None,
                       contract_id: str|None = None,
                       host: str|None = None,
                       thread_id: int|None = None,
                       source_path: str|None = None,
                       source_line: int|None = None,
                       function_name: str|None = None,
                       log_scope: str|None = None,
                       **kwargs):

        if source_path:
            source_path = os.path.relpath(source_path, self.pathbase)

        return Event(
            text          = text,
            domain        = domain,
            origin        = origin or self.identity,
            level         = encodeLogLevel(level),
            code          = code,
            symbol        = symbol,
            timestamp     = encodeTimestamp(timestamp or time.time()),
            attributes    = encodeValueList(kwargs),
            contract_id   = contract_id,
            host          = host or socket.gethostname(),
            thread_id     = thread_id,
            log_scope     = log_scope or "python",
            source_path   = source_path,
            source_line   = source_line,
            function_name = function_name)

    @abc.abstractmethod
    def submit(self, message: Event):
        pass
