'''
logbase.py - Mix-in class to initialize logging
'''

import inspect
import logging
import sys
from .settingsstore import SettingsStore


# if sys.hexversion >= 0x030c0000:
#     # Python 3.12 and above have `taskName`
#     DEFAULT_FORMAT = "%(asctime)s|%(levelname)8s|%(taskName)-8s|%(name)-24s|%(message)s"
# else:
DEFAULT_FORMAT = "%(asctime)s|%(levelname)8s|%(threadName)-12s|%(name)-24s|%(message)s"

def init_logging(threshold: int = logging.INFO,
                 format: str = DEFAULT_FORMAT,
                 force: bool = True):
    logging.basicConfig(level=threshold, format=format, force=force)



class LogBase:
    _log_name = None
    _logger   = None
    _settings = SettingsStore('logging')

    def __init__(self, logger: logging.Logger|None = None):
        self.logger = logger

    @property
    def logger(self) -> logging.Logger:
        '''
        Return an existing or new logger for this instance.
        '''
        if self._logger is None:
            self._logger = logging.getLogger(self.log_name)
            if level := self.threshold_setting(self.log_name):
                self._logger.setLevel(level)
        return self._logger

    @logger.setter
    def logger(self, logger: logging.Logger):
        '''
        Assign a logger to this instance.
        '''
        self._logger = logger

    @property
    def log_name(self) -> str:
        '''
        Return a log name for this object instance.  If this has not yet
        been set, assign a new name based on the Python package name as well as
        this class.
        '''
        if self._log_name is None:
            self._log_name = self.package_name(hops=2)

        return self._log_name

    @log_name.setter
    def log_name(self, log_name: str):
        '''
        Assign a custom name to this logger.
        '''
        self._log_name = log_name

    def package_name(self, hops=1):
        try:
            return inspect.stack()[hops+1].frame.f_globals['__package__']
        except (IndexError, KeyError):
            return '.'.join(self.__module__.split('.')[:-1])

    @classmethod
    def threshold_setting(cls, log_name: str) -> int|None:
        if level_name := cls._settings.get_value(path = ('thresholds', log_name)):
            try:
                level = getattr(logging, level_name)
                assert isinstance(level, int)
            except (AttributeError, AssertionError):
                return None
            else:
                return level
        else:
            return None
